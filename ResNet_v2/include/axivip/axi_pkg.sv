`ifndef AXI_PKG_SV
`define AXI_PKG_SV

package axi_pkg;
import uvm_pkg::*;
`include "uvm_macros.svh"

`include "axi_type.sv"
`include "axi_cfg.sv"
`include "axi_info.sv"
`include "axi_transfer.sv"
`include "axi_driver_base.sv"
`include "axi_state.sv"
`include "axi_monitor.sv"
`include "axi_sequencer.sv"

`include "axi_slave_awdrv.sv"
`include "axi_slave_wdrv.sv"
`include "axi_slave_bdrv.sv"
`include "axi_slave_ardrv.sv"
`include "axi_slave_rdrv.sv"
`include "axi_slave_agent.sv"

`include "axi_master_awdrv.sv"
`include "axi_master_wdrv.sv"
`include "axi_master_bdrv.sv"
`include "axi_master_ardrv.sv"
`include "axi_master_rdrv.sv"
`include "axi_master_agent.sv"

`include "axi_virtual_sequencer.sv"
`include "axi_env.sv"
`include "axi_base_sequence.sv"
`include "axi_slave_seq_lib.sv"
`include "axi_master_seq_lib.sv"
endpackage
`endif
