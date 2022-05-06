`ifndef AXI_CFG_SV
`define AXI_CFG_SV
class axi_cfg extends uvm_object;
    drv_type_enum drv_type = MON_ALONE;
    bit has_checker;
    bit has_coverage;
    int id_num;
    reset_level_enum reset_level;
    axi_latency clatency;
    latency_mode_enum write_latency_mode;
    latency_mode_enum read_latency_mode;

    `uvm_object_utils_begin(axi_cfg)
        `uvm_field_int(has_checker,     UVM_DEFAULT)
        `uvm_field_int(has_coverage,    UVM_DEFAULT)
        `uvm_field_int(id_num,          UVM_DEFAULT)
        `uvm_field_enum(reset_level_enum,   reset_level,        UVM_DEFAULT)
        `uvm_field_enum(drv_type_enum,      drv_type,           UVM_DEFAULT)
        `uvm_field_enum(latency_mode_enum,  write_latency_mode, UVM_DEFAULT)
        `uvm_field_enum(latency_mode_enum,  read_latency_mode, UVM_DEFAULT)
        `uvm_field_object(clatency,     UVM_DEFAULT)
    `uvm_object_utils_end

    function new (string name = "axi_cfg");
        super.new(name);
    endfunction

    function void set_default();
        drv_type    = MON_ALONE;
        has_checker = 1;
        has_coverage= 1;
        id_num      = 1;
        reset_level = RESET_LEVEL_LOW;
        write_latency_mode= CHANNEL_FIRST;
        read_latency_mode= CHANNEL_FIRST;
        clatency    = axi_pkg::axi_latency::type_id::create( "clatency" );
    endfunction
endclass
`endif
