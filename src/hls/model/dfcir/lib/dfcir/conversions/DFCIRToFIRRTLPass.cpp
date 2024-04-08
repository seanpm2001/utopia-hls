#include "dfcir/conversions/DFCIRPasses.h"
#include "dfcir/conversions/DFCIRPassesUtils.h"
#include "circt/Dialect/FIRRTL/FIRRTLDialect.h"
#include "circt/Dialect/FIRRTL/FIRRTLOps.h"
#include "circt/Dialect/FIRRTL/FIRRTLTypes.h"
#include "circt/Dialect/FIRRTL/Passes.h"
#include "circt/Support/LLVM.h"
#include "mlir/IR/Dialect.h"
#include "mlir/Transforms/DialectConversion.h"

namespace mlir::dfcir {

#define GEN_PASS_DECL_DFCIRTOFIRRTLPASS
#define GEN_PASS_DEF_DFCIRTOFIRRTLPASS
#include "dfcir/conversions/DFCIRPasses.h.inc"

    class FIRRTLTypeConverter : public TypeConverter {
    public:
        FIRRTLTypeConverter() {
            addConversion([](Type type) -> Type { return type; });
            addConversion([](DFCIRStreamType type) -> circt::firrtl::IntType {
                Type streamType = type.getStreamType();
                if (streamType.isa<DFCIRFixedType>()) {
                    DFCIRFixedType fixedType = llvm::cast<DFCIRFixedType>(streamType);
                    unsigned width = fixedType.getIntegerBits() + fixedType.getFractionBits();
                    if (fixedType.getSign()) {
                        return circt::firrtl::SIntType::get(fixedType.getContext(), width);
                    } else {
                        return circt::firrtl::UIntType::get(fixedType.getContext(), width);
                    }
                } else if (streamType.isa<DFCIRFloatType>()) {
                    DFCIRFloatType floatType = llvm::cast<DFCIRFloatType>(streamType);
                    unsigned width = floatType.getExponentBits() + floatType.getFractionBits();
                    return circt::firrtl::UIntType::get(floatType.getContext(), width);
                }
                return {};
            });
        }
    };

    template <typename OperationType>
    class FIRRTLOpConversionPattern : public OpConversionPattern<OperationType> {
    public:
        using OpConversionPattern<OperationType>::OpConversionPattern;
        using ConvertedOps = mlir::DenseSet<mlir::Operation *>;

        mutable ConvertedOps *convertedOps;
        const LatencyConfig *latencyConfig;

        FIRRTLOpConversionPattern(MLIRContext *context,
                                  TypeConverter &typeConverter,
                                  ConvertedOps *convertedOps,
                                  LatencyConfig *latencyConfig)
                : OpConversionPattern<OperationType>(typeConverter, context),
                  convertedOps(convertedOps),
                  latencyConfig(latencyConfig) {
            // Required to allow root updates, which imply recursive
            // pattern application.
            //Pattern::setHasBoundedRewriteRecursion(true);
            this->setHasBoundedRewriteRecursion(true);
        }
    };



    class KernelOpConversionPattern : public FIRRTLOpConversionPattern<KernelOp> {
    public:
        using FIRRTLOpConversionPattern<KernelOp>::FIRRTLOpConversionPattern;
        using OpAdaptor = typename KernelOp::Adaptor;
        using CircuitOp = circt::firrtl::CircuitOp;
        using FModuleOp = circt::firrtl::FModuleOp;
        using ConventionAttr = circt::firrtl::ConventionAttr;
        using PortInfo = circt::firrtl::PortInfo;
        using SpecialConstantOp = circt::firrtl::SpecialConstantOp;
        using ClockType = circt::firrtl::ClockType;
        using InputOp = mlir::dfcir::InputOp;
        using OutputOp = mlir::dfcir::OutputOp;

        LogicalResult matchAndRewrite(KernelOp kernelOp, OpAdaptor adaptor,
                                      ConversionPatternRewriter &rewriter) const override {
            Block *kernelBlock = &(kernelOp.getBodyRegion().getBlocks().front());
            auto save = rewriter.saveInsertionPoint();

            // Create a new circuit to substitute the kernel with.
            auto circuitOp = rewriter.create<CircuitOp>(
                    kernelOp.getLoc(),
                    rewriter.getStringAttr(kernelOp.getName()));

            // Collect info on inputs and outputs.
            SmallVector<Operation *> ports;
            SmallVector<circt::firrtl::PortInfo> modulePorts;
            for (Operation &op : kernelBlock->getOperations()) {
                if (llvm::isa<InputOp, OutputOp>(op)) {
                    ports.push_back(&op);
                    Type converted = getTypeConverter()->convertType(op.getResult(0).getType());
                    if (llvm::isa<InputOp>(op)) {
                        modulePorts.emplace_back(
                                mlir::StringAttr::get(getContext(), llvm::cast<InputOp>(op).getName()),
                                converted,
                                circt::firrtl::Direction::In);
                    } else {
                        modulePorts.emplace_back(
                                mlir::StringAttr::get(getContext(), llvm::cast<OutputOp>(op).getName()),
                                converted,
                                circt::firrtl::Direction::Out);
                    }
                }
            }

            // Add explicit clock argument.

            modulePorts.emplace_back(
                    mlir::StringAttr::get(rewriter.getContext(), CLOCK_ARG),
                    circt::firrtl::ClockType::get(rewriter.getContext()),
                    circt::firrtl::Direction::In);

            // Add a module to represent the old kernel with.
            rewriter.setInsertionPointToStart(circuitOp.getBodyBlock());
            auto fModuleOp = rewriter.create<FModuleOp>(
                    rewriter.getUnknownLoc(),
                    StringAttr::get(rewriter.getContext(), kernelOp.getName()),
                    ConventionAttr::get(rewriter.getContext(), Convention::Internal),
                    modulePorts);

            // Replace the input-/output-operations' results with block arguments.

            for (size_t index = 0; index < ports.size(); ++index) {
                BlockArgument arg = fModuleOp.getArgument(index);
                for (auto &operand : llvm::make_early_inc_range(ports[index]->getResult(0).getUses())) {
                    operand.set(arg);
                }
                // TODO: Maybe move to specific pattern?
                rewriter.eraseOp(ports[index]);
            }

            // Empty arguments assumed.
            rewriter.mergeBlocks(kernelBlock,
                                 fModuleOp.getBodyBlock()
                                 //,fModuleOp.getBodyBlock()->getArguments()
                                 );
            rewriter.restoreInsertionPoint(save);
            rewriter.replaceOp(kernelOp, circuitOp);

            return mlir::success();
        }
    };

    template<typename OperationType, typename AdaptorType>
    class SchedulableOpConversionPattern {
        using FExtModuleOp = circt::firrtl::FExtModuleOp;
        using InstanceOp = circt::firrtl::InstanceOp;
        using CircuitOp = circt::firrtl::CircuitOp;


        virtual std::string constructModuleName(const OperationType &op, AdaptorType &adaptor) const = 0;
        virtual FExtModuleOp createModule(const std::string &name, const OperationType &op, AdaptorType &adaptor, ConversionPatternRewriter &rewriter) const = 0;
        virtual void remapUses(OperationType &oldOp, AdaptorType &adaptor, InstanceOp &newOp, ConversionPatternRewriter &rewriter) const = 0;

    protected:
        FExtModuleOp findOrCreateModule(const OperationType &op, AdaptorType &adaptor,
                                        ConversionPatternRewriter &rewriter) const {
            std::string moduleName = constructModuleName(op, adaptor);
            CircuitOp circuit = circt::firrtl::utils::findCircuit(op);

            auto foundModule = circuit.template lookupSymbol<FExtModuleOp>(moduleName);
            if (foundModule) return foundModule;

            auto saved = rewriter.saveInsertionPoint();
            rewriter.setInsertionPointToStart(circuit.getBodyBlock());
            FExtModuleOp newModule = createModule(moduleName, op, adaptor, rewriter);
            rewriter.restoreInsertionPoint(saved);
            return newModule;
        }

    };

    class AddOpConversionPattern : public FIRRTLOpConversionPattern<AddOp>,
                                          SchedulableOpConversionPattern<AddOp,
                                                                         AddOp::Adaptor> {
    public:
        using FIRRTLOpConversionPattern<AddOp>::FIRRTLOpConversionPattern;
        using OpAdaptor = typename AddOp::Adaptor;
        using FExtModuleOp = circt::firrtl::FExtModuleOp;
        using InstanceOp = circt::firrtl::InstanceOp;
        using ConnectOp = circt::firrtl::ConnectOp;
        using IntType = circt::firrtl::IntType;
        using SIntType = circt::firrtl::SIntType;
        using UIntType = circt::firrtl::UIntType;

        std::string constructModuleName(const AddOp &op, OpAdaptor &adaptor) const override {
            Type type = op->getResult(0).getType();
            Type convType = getTypeConverter()->convertType(type);

            bool isFloat = false;

            std::string name = ADD_MODULE"_";
            llvm::raw_string_ostream nameStream(name);

            if ((isFloat = type.isa<DFCIRFloatType>())) {
                nameStream << FLOAT_SPEC"_";
            } else if (convType.isa<IntType>()) {
                nameStream << INT_SPEC"_";
            }
            unsigned latency;
            if (isFloat) {
                DFCIRFloatType casted = llvm::cast<DFCIRFloatType>(type);
                nameStream << (casted.getExponentBits() + casted.getFractionBits()) << "#" << casted.getExponentBits();
                latency = latencyConfig->find(ADD_FLOAT)->second;
            } else {
                nameStream << llvm::cast<IntType>(convType).getWidthOrSentinel() << "_";
                latency = latencyConfig->find(ADD_INT)->second;
            }
            nameStream << "##" << latency;
            return name;
        }

        FExtModuleOp createModule(const std::string &name, const AddOp &op, OpAdaptor &adaptor,
                                  ConversionPatternRewriter &rewriter) const override {
            Type type = op->getResult(0).getType();
            Type converted = getTypeConverter()->convertType(type);
            SmallVector<circt::firrtl::PortInfo> ports = {
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "out"),
                            converted,
                            circt::firrtl::Direction::Out),
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "first"),
                            adaptor.getFirst().getType(),
                            circt::firrtl::Direction::In),
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "second"),
                            adaptor.getSecond().getType(),
                            circt::firrtl::Direction::In),
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "clk"),
                            circt::firrtl::ClockType::get(rewriter.getContext()),
                            circt::firrtl::Direction::In)
            };
            IntegerType attrType = mlir::IntegerType::get(rewriter.getContext(), 32, mlir::IntegerType::Unsigned);
            auto outTypeWidth = circt::firrtl::getBitWidth(llvm::dyn_cast<circt::firrtl::FIRRTLBaseType>(converted));
            assert(outTypeWidth.has_value());
            auto firstTypeWidth = circt::firrtl::getBitWidth(llvm::dyn_cast<circt::firrtl::FIRRTLBaseType>(adaptor.getFirst().getType()));
            assert(firstTypeWidth.has_value());
            auto secondTypeWidth = circt::firrtl::getBitWidth(llvm::dyn_cast<circt::firrtl::FIRRTLBaseType>(adaptor.getSecond().getType()));
            assert(secondTypeWidth.has_value());
            assert(*outTypeWidth == *firstTypeWidth && *outTypeWidth == *secondTypeWidth);

            bool isFloat = type.isa<DFCIRFloatType>();
            unsigned latency = latencyConfig->find((isFloat) ? ADD_FLOAT : ADD_INT)->second;
            auto module = rewriter.create<FExtModuleOp>(
                    rewriter.getUnknownLoc(),
                    mlir::StringAttr::get(rewriter.getContext(), name),
                    circt::firrtl::ConventionAttr::get(rewriter.getContext(), Convention::Internal),
                    ports,
                    StringRef((isFloat ? (ADD_MODULE "_" FLOAT_SPEC) : (ADD_MODULE "_" INT_SPEC))),
                    mlir::ArrayAttr(),
                    mlir::ArrayAttr::get(rewriter.getContext(),
                        {
                            circt::firrtl::ParamDeclAttr::get(rewriter.getContext(),
                                mlir::StringAttr::get(rewriter.getContext(), STAGES_PARAM),
                                attrType,
                                mlir::IntegerAttr::get(attrType, latency)),
                            circt::firrtl::ParamDeclAttr::get(rewriter.getContext(),
                                mlir::StringAttr::get(rewriter.getContext(), "op_" TYPE_SIZE_PARAM),
                                attrType,
                                mlir::IntegerAttr::get(attrType, *outTypeWidth))
                        }));
            module->setAttr(INSTANCE_LATENCY_ATTR, mlir::IntegerAttr::get(attrType, latency));
            return module;
        }

        void remapUses(AddOp &oldOp, OpAdaptor &adaptor,
                       InstanceOp &newOp, ConversionPatternRewriter &rewriter) const override {
            rewriter.create<ConnectOp>(rewriter.getUnknownLoc(), newOp.getResult(1), adaptor.getFirst());
            rewriter.create<ConnectOp>(rewriter.getUnknownLoc(), newOp.getResult(2), adaptor.getSecond());
            rewriter.create<ConnectOp>(rewriter.getUnknownLoc(),
                                       newOp.getResult(3),
                                       circt::firrtl::utils::getClockVarFromOpBlock(newOp));
            for (auto &operand : llvm::make_early_inc_range(oldOp.getRes().getUses())) {
                operand.set(newOp.getResult(0));
            }
        }


        LogicalResult matchAndRewrite(AddOp addOp, OpAdaptor adaptor,
                                      ConversionPatternRewriter &rewriter) const override {
            FExtModuleOp module = findOrCreateModule(addOp, adaptor, rewriter);

            InstanceOp newOp = rewriter.create<InstanceOp>(
                    addOp.getLoc(),
                    module,
                    "placeholder");

            remapUses(addOp, adaptor, newOp, rewriter);

            rewriter.eraseOp(addOp);

            return mlir::success();
        }
    };

    class MulOpConversionPattern : public FIRRTLOpConversionPattern<MulOp>,
                                          SchedulableOpConversionPattern<MulOp, MulOp::Adaptor> {
    public:
        using FIRRTLOpConversionPattern<MulOp>::FIRRTLOpConversionPattern;
        using OpAdaptor = typename MulOp::Adaptor;
        using FExtModuleOp = circt::firrtl::FExtModuleOp;
        using InstanceOp = circt::firrtl::InstanceOp;
        using ConnectOp = circt::firrtl::ConnectOp;
        using IntType = circt::firrtl::IntType;

        std::string constructModuleName(const MulOp &op, OpAdaptor &adaptor) const override {
            Type type = op->getResult(0).getType();
            Type convType = getTypeConverter()->convertType(type);

            bool isFloat = false;

            std::string name = MUL_MODULE"_";
            llvm::raw_string_ostream nameStream(name);

            if ((isFloat = type.isa<DFCIRFloatType>())) {
                nameStream << FLOAT_SPEC"_";
            } else if (convType.isa<IntType>()) {
                nameStream << INT_SPEC"_";
            }

            unsigned latency;
            if (isFloat) {
                DFCIRFloatType casted = llvm::cast<DFCIRFloatType>(type);
                nameStream << (casted.getExponentBits() + casted.getFractionBits()) << "#" << casted.getExponentBits();
                latency = latencyConfig->find(MUL_FLOAT)->second;
            } else {
                nameStream << llvm::cast<IntType>(convType).getWidthOrSentinel() << "_";
                latency = latencyConfig->find(MUL_INT)->second;
            }

            nameStream << "##" << latency;
            return name;
        }

        FExtModuleOp createModule(const std::string &name, const MulOp &op, OpAdaptor &adaptor,
                                  ConversionPatternRewriter &rewriter) const override {
            Type type = op->getResult(0).getType();
            Type converted = getTypeConverter()->convertType(type);
            SmallVector<circt::firrtl::PortInfo> ports = {
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "out"),
                            converted,
                            circt::firrtl::Direction::Out),
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "first"),
                            adaptor.getFirst().getType(),
                            circt::firrtl::Direction::In),
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "second"),
                            adaptor.getSecond().getType(),
                            circt::firrtl::Direction::In),
                    circt::firrtl::PortInfo(
                            mlir::StringAttr::get(rewriter.getContext(), "clk"),
                            circt::firrtl::ClockType::get(rewriter.getContext()),
                            circt::firrtl::Direction::In)
            };
            IntegerType attrType = mlir::IntegerType::get(rewriter.getContext(), 32, mlir::IntegerType::Unsigned);
            auto outTypeWidth = circt::firrtl::getBitWidth(llvm::dyn_cast<circt::firrtl::FIRRTLBaseType>(converted));
            assert(outTypeWidth.has_value());
            auto firstTypeWidth = circt::firrtl::getBitWidth(llvm::dyn_cast<circt::firrtl::FIRRTLBaseType>(adaptor.getFirst().getType()));
            assert(firstTypeWidth.has_value());
            auto secondTypeWidth = circt::firrtl::getBitWidth(llvm::dyn_cast<circt::firrtl::FIRRTLBaseType>(adaptor.getSecond().getType()));
            assert(secondTypeWidth.has_value());
            assert(*outTypeWidth == *firstTypeWidth && *outTypeWidth == *secondTypeWidth);

            bool isFloat = type.isa<DFCIRFloatType>();
            unsigned latency = latencyConfig->find((isFloat) ? MUL_FLOAT : MUL_INT)->second;
            auto module = rewriter.create<FExtModuleOp>(
                    rewriter.getUnknownLoc(),
                    mlir::StringAttr::get(rewriter.getContext(), name),
                    circt::firrtl::ConventionAttr::get(rewriter.getContext(), Convention::Internal),
                    ports,
                    StringRef((isFloat ? (MUL_MODULE "_" FLOAT_SPEC) : (MUL_MODULE "_" INT_SPEC))),
                    mlir::ArrayAttr(),
                    mlir::ArrayAttr::get(rewriter.getContext(),
                                         {
                                                 circt::firrtl::ParamDeclAttr::get(rewriter.getContext(),
                                                                                   mlir::StringAttr::get(rewriter.getContext(), STAGES_PARAM),
                                                                                   attrType,
                                                                                   mlir::IntegerAttr::get(attrType, latency)),
                                                 circt::firrtl::ParamDeclAttr::get(rewriter.getContext(),
                                                                                   mlir::StringAttr::get(rewriter.getContext(), "op_" TYPE_SIZE_PARAM),
                                                                                   attrType,
                                                                                   mlir::IntegerAttr::get(attrType, *outTypeWidth))
                                         }));
            module->setAttr(INSTANCE_LATENCY_ATTR, mlir::IntegerAttr::get(attrType, latency));
            return module;
        }

        void remapUses(MulOp &oldOp, OpAdaptor &adaptor, InstanceOp &newOp, ConversionPatternRewriter &rewriter) const override {
            rewriter.create<ConnectOp>(rewriter.getUnknownLoc(), newOp.getResult(1), adaptor.getFirst());
            rewriter.create<ConnectOp>(rewriter.getUnknownLoc(), newOp.getResult(2), adaptor.getSecond());
            rewriter.create<ConnectOp>(rewriter.getUnknownLoc(),
                                       newOp.getResult(3),
                                       circt::firrtl::utils::getClockVarFromOpBlock(newOp));
            for (auto &operand : llvm::make_early_inc_range(oldOp.getRes().getUses())) {
                operand.set(newOp.getResult(0));
            }
        }


        LogicalResult matchAndRewrite(MulOp mulOp, OpAdaptor adaptor,
                                      ConversionPatternRewriter &rewriter) const override {
            // unsigned latency = latencyConfig->find(MUL)->second;
            FExtModuleOp module = findOrCreateModule(mulOp, adaptor, rewriter);

            InstanceOp newOp = rewriter.create<InstanceOp>(
                    mulOp.getLoc(),
                    module,
                    "placeholder");

            remapUses(mulOp, adaptor, newOp, rewriter);

            rewriter.eraseOp(mulOp);

            return mlir::success();
        }
    };

    class ConnectOpConversionPattern : public FIRRTLOpConversionPattern<ConnectOp> {
    public:
        using FIRRTLOpConversionPattern<ConnectOp>::FIRRTLOpConversionPattern;
        using OpAdaptor = typename ConnectOp::Adaptor;

        LogicalResult matchAndRewrite(ConnectOp connectOp, OpAdaptor adaptor,
                                      ConversionPatternRewriter &rewriter) const override {
            auto newOp = rewriter.create<circt::firrtl::ConnectOp>(
                    connectOp.getLoc(),
                    adaptor.getConnecting(),
                    adaptor.getConnectee()
            );
            rewriter.replaceOp(connectOp, newOp);
            return mlir::success();
        }
    };

    class DFCIRToFIRRTLPass : public impl::DFCIRToFIRRTLPassBase<DFCIRToFIRRTLPass> {
    public:
        using ConvertedOps = mlir::DenseSet<mlir::Operation *>;

        explicit DFCIRToFIRRTLPass(const DFCIRToFIRRTLPassOptions &options)
                : impl::DFCIRToFIRRTLPassBase<DFCIRToFIRRTLPass>(options) { }

        void runOnOperation() override {
            // Define the conversion target.
            ConversionTarget target(getContext());
            target.addLegalDialect<DFCIRDialect>();
            target.addIllegalOp<KernelOp>();
            target.addLegalDialect<circt::firrtl::FIRRTLDialect>();

            // TODO: Implement 'FIRRTLTypeConverter' completely.
            FIRRTLTypeConverter typeConverter;
            ConvertedOps convertedOps;

            // Convert the kernel first to get a FIRRTL-circuit.
            RewritePatternSet patterns(&getContext());

            patterns.add<KernelOpConversionPattern>(
                    &getContext(),
                    typeConverter,
                    &convertedOps,
                    latencyConfig);

            // Apply partial conversion.
            if (failed(applyPartialConversion(getOperation(),
                                              target,
                                              std::move(patterns)))) {
                signalPassFailure();
            }

            // Define the rest of the rewrite patterns.
            patterns.clear();
            target.addIllegalDialect<DFCIRDialect>();
            target.addIllegalOp<UnrealizedConversionCastOp>();
            patterns.add<
                    AddOpConversionPattern,
                    MulOpConversionPattern,
                    ConnectOpConversionPattern>(
                    &getContext(),
                    typeConverter,
                    &convertedOps,
                    latencyConfig);

            // Apply partial conversion.
            if (failed(applyPartialConversion(getOperation(),
                                              target,
                                              std::move(patterns)))) {
                signalPassFailure();
            }
        }
    };

    std::unique_ptr<mlir::Pass> createDFCIRToFIRRTLPass(LatencyConfig *config) {
        DFCIRToFIRRTLPassOptions options;
        options.latencyConfig = config;
        return std::make_unique<DFCIRToFIRRTLPass>(options);
    }

} // namespace mlir::dfcir