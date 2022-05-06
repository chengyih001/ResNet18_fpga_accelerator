`ifndef AXI_SLAVE_ARDRV_SV
`define AXI_SLAVE_ARDRV_SV

class axi_slave_ardrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_slave_ardrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual task reset_signals();
        vif.ARREADY <= 0;
    endtask

    virtual task get_and_drive();
        forever begin
            seq_item_port.get_next_item(req);
            if (req.addr_delay >= 0) begin
                wait(vif.ARVALID);
                #0;
                repeat(req.addr_delay) @(posedge vif.clk);
                if (req.addr_delay > 0) #0;
            end
            vif.ARREADY = 1'b1;
            `uvm_info (this.get_full_name(), "set vif.arready = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.ARREADY == 1) && (vif.ARVALID == 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.arready = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.ARREADY = 1'b0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


