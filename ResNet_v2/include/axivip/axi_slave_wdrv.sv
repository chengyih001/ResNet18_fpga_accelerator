`ifndef AXI_SLAVE_WDRV_SV
`define AXI_SLAVE_WDRV_SV

class axi_slave_wdrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_slave_wdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual task reset_signals();
        vif.WREADY <= 0;
    endtask

    virtual task get_and_drive();
        forever begin
            seq_item_port.get_next_item(req);
            if (req.data_delay[0] >= 0) begin
                wait(vif.WVALID);
                #0;
                repeat(req.data_delay[0]) @(posedge vif.clk);
                if (req.data_delay[0] > 0) #0;
            end
            vif.WREADY = 1'b1;
            `uvm_info (this.get_full_name(), "set vif.wready = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.WREADY == 1) && (vif.WVALID == 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.wready = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.WREADY = 1'b0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


