`ifndef COSIM_TRACE_PACKAGE__SV
`define COSIM_TRACE_PACKAGE__SV
package cosim_trace_package;
    `include "trace_base_component.sv"
    `include "vld_rdy_data_trace_phy_mondrv.sv"
    `include "axi_trace_phy_mondrv.sv"
    `include "axi_trace_collect.sv"
    `include "mem_trace_manager.sv"
    `include "cmd_trace_manager.sv"
    `include "maxi_trace_agent.sv"
    `include "axilite_trace_agent.sv"
endpackage

//cat trace_base_component.sv vld_rdy_data_trace_phy_mondrv.sv axi_trace_phy_mondrv.sv axi_trace_collect.sv mem_trace_manager.sv cmd_trace_manager.sv maxi_trace_agent.sv axilite_trace_agent.sv > aa_hwemu_trace_combine.sv
`endif
