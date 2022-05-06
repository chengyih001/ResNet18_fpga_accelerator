`ifndef AXI_INFO__SV
`define AXI_INFO__SV

class axi_info extends uvm_object;
    hs_event_enum dutevt;
    int trans_size;

    `uvm_object_utils_begin(axi_info)
        `uvm_field_enum(hs_event_enum, dutevt, UVM_DEFAULT);
        `uvm_field_int(trans_size, UVM_DEFAULT);
    `uvm_object_utils_end

    function new (string name = "axi_info");
        super.new(name);
    endfunction
endclass

`endif
