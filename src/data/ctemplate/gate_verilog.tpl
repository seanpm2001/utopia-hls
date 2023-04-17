{{#LICENSE}}
{{!===---------------------------------------------------------------------===}}
{{!                                                                           }}
{{! Part of the Utopia EDA Project, under the Apache License v2.0             }}
{{! SPDX-License-Identifier: Apache-2.0                                       }}
{{! Copyright 2023 ISP RAS (http://www.ispras.ru)                             }}
{{!                                                                           }}
{{!===---------------------------------------------------------------------===}}
{{/LICENSE}}
// This file has been automatically generated at
// {{GEN_TIME}}
{{#MODULES}}
module {{MODULE_NAME}} (
// outputs
{{#OUTS}}output	{{OUTPUT}}{{SEP}}
{{/OUTS}}
// inputs
{{#INS}}input {{INPUT}}{{SEP}}
{{/INS}});
// wires
{{#WIRES}}wire {{WIRE_NAME}};
{{/WIRES}}
// assignments
{{#ASSIGNS}}assign {{LHS}} = {{RHS}};
{{/ASSIGNS}}
// gates
{{#GATES}}{{GATE_TYPE}} {{GATE_NAME}}({{GATE_OUT}}{{#G_INS}}, {{GATE_IN}}{{/G_INS}});
{{/GATES}}

endmodule // {{MODULE_NAME}}
{{/MODULES}}
