
// This MLIR FIRRTL DIALECT top module was automatically generated by Utopia EDA
// Generation started: 29-1-2023 8:52:56
 
// Ivannikov Institute for System Programming
// of the Russian Academy of Sciences (ISP RAS)
// 25 Alexander Solzhenitsyn st., Moscow, 109004, Russia
// http://forge.ispras.ru/projects/utopia

firrtl.circuit "ScalarMul" {
    firrtl.module @ScalarMul (
    in %clock : !firrtl.clock,
    in %reset : !firrtl.reset,
    in %source__dfc_wire_4853__dfc_wire_4853 : !firrtl.sint<16>,
    in %source__dfc_wire_4857__dfc_wire_4853 : !firrtl.sint<16>,
    in %source__dfc_wire_4854__dfc_wire_4853 : !firrtl.sint<16>,
    in %source__dfc_wire_4855__dfc_wire_4853 : !firrtl.sint<16>,
    in %source__dfc_wire_4856__dfc_wire_4853 : !firrtl.sint<16>,
    in %source__dfc_wire_4852__dfc_wire_4853 : !firrtl.sint<16>,

    out %sink__dfc_wire_4873__dfc_wire_4873 : !firrtl.sint<16>
)
    {
    // Instances
    %MUL2166_clock, %MUL2166_reset, %MUL2166__dfc_wire_4852, %MUL2166__dfc_wire_4855,  %MUL2166__dfc_wire_4859 = firrtl.instance MUL2166 @MUL_2x1(in clock : !firrtl.clock, in reset : !firrtl.reset, in _dfc_wire_4852 : !firrtl.sint<16>, in _dfc_wire_4855 : !firrtl.sint<16>, out _dfc_wire_4859 : !firrtl.sint<16> )
    %MUL2167_clock, %MUL2167_reset, %MUL2167__dfc_wire_4852, %MUL2167__dfc_wire_4855,  %MUL2167__dfc_wire_4859 = firrtl.instance MUL2167 @MUL_2x1(in clock : !firrtl.clock, in reset : !firrtl.reset, in _dfc_wire_4852 : !firrtl.sint<16>, in _dfc_wire_4855 : !firrtl.sint<16>, out _dfc_wire_4859 : !firrtl.sint<16> )
    %ADD2168_clock, %ADD2168_reset, %ADD2168__dfc_wire_4859, %ADD2168__dfc_wire_4863,  %ADD2168__dfc_wire_4866 = firrtl.instance ADD2168 @ADD_2x1(in clock : !firrtl.clock, in reset : !firrtl.reset, in _dfc_wire_4859 : !firrtl.sint<16>, in _dfc_wire_4863 : !firrtl.sint<16>, out _dfc_wire_4866 : !firrtl.sint<16> )
    %MUL2169_clock, %MUL2169_reset, %MUL2169__dfc_wire_4852, %MUL2169__dfc_wire_4855,  %MUL2169__dfc_wire_4859 = firrtl.instance MUL2169 @MUL_2x1(in clock : !firrtl.clock, in reset : !firrtl.reset, in _dfc_wire_4852 : !firrtl.sint<16>, in _dfc_wire_4855 : !firrtl.sint<16>, out _dfc_wire_4859 : !firrtl.sint<16> )
    %ADD2170_clock, %ADD2170_reset, %ADD2170__dfc_wire_4859, %ADD2170__dfc_wire_4863,  %ADD2170__dfc_wire_4866 = firrtl.instance ADD2170 @ADD_2x1(in clock : !firrtl.clock, in reset : !firrtl.reset, in _dfc_wire_4859 : !firrtl.sint<16>, in _dfc_wire_4863 : !firrtl.sint<16>, out _dfc_wire_4866 : !firrtl.sint<16> )
    %delay_fixed_16_0_1_8_12_clock, %delay_fixed_16_0_1_8_12_reset, %delay_fixed_16_0_1_8_12_in,  %delay_fixed_16_0_1_8_12_out = firrtl.instance delay_fixed_16_0_1_8_12 @delay_fixed_16_0_1_8(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<16>, out out : !firrtl.sint<16> )
    %delay_fixed_16_0_1_66_13_clock, %delay_fixed_16_0_1_66_13_reset, %delay_fixed_16_0_1_66_13_in,  %delay_fixed_16_0_1_66_13_out = firrtl.instance delay_fixed_16_0_1_66_13 @delay_fixed_16_0_1_66(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<16>, out out : !firrtl.sint<16> )
    %delay_fixed_16_0_1_12_14_clock, %delay_fixed_16_0_1_12_14_reset, %delay_fixed_16_0_1_12_14_in,  %delay_fixed_16_0_1_12_14_out = firrtl.instance delay_fixed_16_0_1_12_14 @delay_fixed_16_0_1_12(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<16>, out out : !firrtl.sint<16> )
    %delay_fixed_16_0_1_22_15_clock, %delay_fixed_16_0_1_22_15_reset, %delay_fixed_16_0_1_22_15_in,  %delay_fixed_16_0_1_22_15_out = firrtl.instance delay_fixed_16_0_1_22_15 @delay_fixed_16_0_1_22(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<16>, out out : !firrtl.sint<16> )
    %delay_fixed_16_0_1_12_16_clock, %delay_fixed_16_0_1_12_16_reset, %delay_fixed_16_0_1_12_16_in,  %delay_fixed_16_0_1_12_16_out = firrtl.instance delay_fixed_16_0_1_12_16 @delay_fixed_16_0_1_12(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<16>, out out : !firrtl.sint<16> )
    
    // Connections
    firrtl.connect %MUL2166_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %MUL2166_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %MUL2166__dfc_wire_4852, %source__dfc_wire_4852__dfc_wire_4853 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %ADD2168__dfc_wire_4859, %MUL2166__dfc_wire_4859 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %MUL2167_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %MUL2167_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %MUL2167__dfc_wire_4855, %source__dfc_wire_4856__dfc_wire_4853 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_12_14_in, %MUL2167__dfc_wire_4859 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %ADD2168_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %ADD2168_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %ADD2170__dfc_wire_4859, %ADD2168__dfc_wire_4866 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %MUL2169_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %MUL2169_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %MUL2169__dfc_wire_4855, %source__dfc_wire_4857__dfc_wire_4853 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_8_12_in, %MUL2169__dfc_wire_4859 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %ADD2170_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %ADD2170_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %sink__dfc_wire_4873__dfc_wire_4873, %ADD2170__dfc_wire_4866 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_8_12_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_fixed_16_0_1_8_12_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %ADD2170__dfc_wire_4863, %delay_fixed_16_0_1_8_12_out : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_66_13_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_fixed_16_0_1_66_13_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %delay_fixed_16_0_1_66_13_in, %source__dfc_wire_4854__dfc_wire_4853 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %MUL2169__dfc_wire_4852, %delay_fixed_16_0_1_66_13_out : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_12_14_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_fixed_16_0_1_12_14_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %ADD2168__dfc_wire_4863, %delay_fixed_16_0_1_12_14_out : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_22_15_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_fixed_16_0_1_22_15_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %delay_fixed_16_0_1_22_15_in, %source__dfc_wire_4853__dfc_wire_4853 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %MUL2167__dfc_wire_4852, %delay_fixed_16_0_1_22_15_out : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %delay_fixed_16_0_1_12_16_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_fixed_16_0_1_12_16_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %delay_fixed_16_0_1_12_16_in, %source__dfc_wire_4855__dfc_wire_4853 : !firrtl.sint<16>, !firrtl.sint<16>
    firrtl.connect %MUL2166__dfc_wire_4855, %delay_fixed_16_0_1_12_16_out : !firrtl.sint<16>, !firrtl.sint<16>
    
    }
    // External modules declaration
    firrtl.extmodule @ADD_2x1(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in _dfc_wire_4859 : !firrtl.sint<16>,
        in _dfc_wire_4863 : !firrtl.sint<16>,
    
        out _dfc_wire_4866 : !firrtl.sint<16>
    
    )
    firrtl.extmodule @MUL_2x1(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in _dfc_wire_4852 : !firrtl.sint<16>,
        in _dfc_wire_4855 : !firrtl.sint<16>,
    
        out _dfc_wire_4859 : !firrtl.sint<16>
    
    )
    firrtl.extmodule @delay_fixed_16_0_1_12(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<16>,
    
        out out : !firrtl.sint<16>
    
    )
    firrtl.extmodule @delay_fixed_16_0_1_22(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<16>,
    
        out out : !firrtl.sint<16>
    
    )
    firrtl.extmodule @delay_fixed_16_0_1_66(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<16>,
    
        out out : !firrtl.sint<16>
    
    )
    firrtl.extmodule @delay_fixed_16_0_1_8(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<16>,
    
        out out : !firrtl.sint<16>
    
    )
    firrtl.extmodule @sink_1x0(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in _dfc_wire_4873 : !firrtl.sint<16>
    
    
    )
    firrtl.extmodule @source_0x1(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
    
        out _dfc_wire_4853 : !firrtl.sint<16>
    
    )
    
}
