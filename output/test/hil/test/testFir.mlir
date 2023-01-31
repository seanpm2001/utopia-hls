
// This MLIR FIRRTL DIALECT top module was automatically generated by Utopia EDA
// Generation started: 29-1-2023 8:52:48
 
// Ivannikov Institute for System Programming
// of the Russian Academy of Sciences (ISP RAS)
// 25 Alexander Solzhenitsyn st., Moscow, 109004, Russia
// http://forge.ispras.ru/projects/utopia

firrtl.circuit "main" {
    firrtl.module @main (
    in %clock : !firrtl.clock,
    in %reset : !firrtl.reset,
    in %n1_x : !firrtl.sint<1>,
    in %n1_y : !firrtl.sint<1>,

    out %n6_z : !firrtl.sint<1>
)
    {
    // Instances
    %n2_clock, %n2_reset, %n2_x,  %n2_x1, %n2_x2 = firrtl.instance n2 @split(in clock : !firrtl.clock, in reset : !firrtl.reset, in x : !firrtl.sint<1>, out x1 : !firrtl.sint<1>, out x2 : !firrtl.sint<1> )
    %n3_clock, %n3_reset, %n3_x, %n3_y,  %n3_z, %n3_w = firrtl.instance n3 @kernel1(in clock : !firrtl.clock, in reset : !firrtl.reset, in x : !firrtl.sint<1>, in y : !firrtl.sint<1>, out z : !firrtl.sint<1>, out w : !firrtl.sint<1> )
    %n4_clock, %n4_reset, %n4_x, %n4_w,  %n4_z = firrtl.instance n4 @kernel2(in clock : !firrtl.clock, in reset : !firrtl.reset, in x : !firrtl.sint<1>, in w : !firrtl.sint<1>, out z : !firrtl.sint<1> )
    %n5_clock, %n5_reset, %n5_z1, %n5_z2,  %n5_z = firrtl.instance n5 @merge(in clock : !firrtl.clock, in reset : !firrtl.reset, in z1 : !firrtl.sint<1>, in z2 : !firrtl.sint<1>, out z : !firrtl.sint<1> )
    %delay_Z_57_6_clock, %delay_Z_57_6_reset, %delay_Z_57_6_in,  %delay_Z_57_6_out = firrtl.instance delay_Z_57_6 @delay_Z_57(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<1>, out out : !firrtl.sint<1> )
    %delay_X_8_7_clock, %delay_X_8_7_reset, %delay_X_8_7_in,  %delay_X_8_7_out = firrtl.instance delay_X_8_7 @delay_X_8(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<1>, out out : !firrtl.sint<1> )
    %delay_Y_1_8_clock, %delay_Y_1_8_reset, %delay_Y_1_8_in,  %delay_Y_1_8_out = firrtl.instance delay_Y_1_8 @delay_Y_1(in clock : !firrtl.clock, in reset : !firrtl.reset, in in : !firrtl.sint<1>, out out : !firrtl.sint<1> )
    
    // Connections
    firrtl.connect %n2_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %n2_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %n2_x, %n1_x : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %n3_x, %n2_x1 : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %delay_X_8_7_in, %n2_x2 : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %n3_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %n3_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %delay_Z_57_6_in, %n3_z : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %n4_w, %n3_w : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %n4_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %n4_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %n5_z2, %n4_z : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %n5_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %n5_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %n6_z, %n5_z : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %delay_Z_57_6_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_Z_57_6_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %n5_z1, %delay_Z_57_6_out : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %delay_X_8_7_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_X_8_7_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %n4_x, %delay_X_8_7_out : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %delay_Y_1_8_clock, %clock : !firrtl.clock, !firrtl.clock
    firrtl.connect %delay_Y_1_8_reset, %reset : !firrtl.reset, !firrtl.reset
    firrtl.connect %delay_Y_1_8_in, %n1_y : !firrtl.sint<1>, !firrtl.sint<1>
    firrtl.connect %n3_y, %delay_Y_1_8_out : !firrtl.sint<1>, !firrtl.sint<1>
    
    }
    // External modules declaration
    firrtl.extmodule @delay_X_8(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<1>,
    
        out out : !firrtl.sint<1>
    
    )
    firrtl.extmodule @delay_Y_1(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<1>,
    
        out out : !firrtl.sint<1>
    
    )
    firrtl.extmodule @delay_Z_57(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in in : !firrtl.sint<1>,
    
        out out : !firrtl.sint<1>
    
    )
    firrtl.extmodule @kernel1(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in x : !firrtl.sint<1>,
        in y : !firrtl.sint<1>,
    
        out z : !firrtl.sint<1>,
        out w : !firrtl.sint<1>
    
    )
    firrtl.extmodule @kernel2(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in x : !firrtl.sint<1>,
        in w : !firrtl.sint<1>,
    
        out z : !firrtl.sint<1>
    
    )
    firrtl.extmodule @merge(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in z1 : !firrtl.sint<1>,
        in z2 : !firrtl.sint<1>,
    
        out z : !firrtl.sint<1>
    
    )
    firrtl.extmodule @sink(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in z : !firrtl.sint<1>
    
    
    )
    firrtl.extmodule @source(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
    
        out x : !firrtl.sint<1>,
        out y : !firrtl.sint<1>
    
    )
    firrtl.extmodule @split(
        in clock : !firrtl.clock,
        in reset : !firrtl.reset,
        in x : !firrtl.sint<1>,
    
        out x1 : !firrtl.sint<1>,
        out x2 : !firrtl.sint<1>
    
    )
    
}
