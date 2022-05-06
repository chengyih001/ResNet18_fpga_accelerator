`ifndef AXI_MASTER_WDRV_SV
`define AXI_MASTER_WDRV_SV

class axi_master_wdrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_master_wdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual task reset_signals();
        vif.WVALID <= 0;
        vif.WID <= 0;
        vif.WSTRB <= 0;
        vif.WLAST <=0;
    endtask

    virtual task get_and_drive();
        forever begin
            seq_item_port.get_next_item(req);
            if (req.data_delay[0] >= 0) begin
                wait(vif.WREADY);
                #0;
                repeat(req.data_delay[0]) @(posedge vif.clk);
                if (req.data_delay[0] > 0) #0;
            end
            vif.WID       = req.id;
            vif.WVALID    = 1'b1;
            vif.WSTRB     = 'h0;
            for(int i=req.addr%STRB_WIDTH; i<STRB_WIDTH; i++) begin
                vif.WDATA[i*8+7 -:8] = req.data.pop_front();
                vif.WSTRB[i]         = 'h1;
            end
            vif.WLAST     = req.last;
            vif.WUSER     = req.user;

            `uvm_info (this.get_full_name(), "set vif.wvalid = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.WREADY == 1) && (vif.WVALID == 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.wvalid = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.WVALID = 1'b0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


