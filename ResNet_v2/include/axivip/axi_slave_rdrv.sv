`ifndef AXI_SLAVE_RDRV_SV
`define AXI_SLAVE_RDRV_SV

class axi_slave_rdrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_slave_rdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual task reset_signals();
        vif.RVALID <= 0;
    endtask

    virtual task get_and_drive();
        forever begin
            int delay;
            seq_item_port.get_next_item(req);
            delay = req.data_delay[0];
            if (delay >= 0) begin
                wait(vif.RREADY);
                #0;
                repeat(delay) @(posedge vif.clk);
                if (delay > 0) #0;
            end
            vif.RVALID = 1'b1;
            for(int i=req.addr%req.bus_bytenum; i<STRB_WIDTH; i++)
                vif.RDATA[i*8+7 -:8]=req.data.pop_front();
            vif.RRESP  = req.rresp[0];
            vif.RLAST  = req.last;
            vif.RID    = req.id;
            `uvm_info (this.get_full_name(), "set vif.rvalid = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.RREADY == 1) && (vif.RVALID == 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.rvalid = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.RVALID = 1'b0;
            vif.RDATA  = 'hx;
            vif.RRESP  = 'h0;
            vif.RLAST  = 'h0;
            //vif.RID    = 'h0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


