`ifndef MAXI_TRACE_AGENT__SV
`define MAXI_TRACE_AGENT__SV

class maxi_trace_monagent#(int ADDRW=64, int DATAW=64, int LENGTHW=8, int SIZEW=3, int IDW=1) extends trace_base_component;

axi_ctr_trace_phy_mondrv#(ADDRW, LENGTHW, SIZEW, IDW) AwPhy;
axi_wdata_trace_phy_mondrv#(DATAW, IDW) WPhy;
axi_brsp_trace_phy_mondrv#(IDW) BPhy;
axi_ctr_trace_phy_mondrv#(ADDRW, LENGTHW, SIZEW, IDW) ArPhy;
axi_rdata_trace_phy_mondrv#(DATAW, IDW) RPhy;

axi_trace_collect Collect;

mem_trace_manager MemMan; //only one memory manager; shared by all maxi interface

function new(string inst_name, string hier_name);
    super.new(inst_name, hier_name);
    AwPhy=new("AwPhy", this.full_name());
     WPhy=new("WPhy",  this.full_name());
     BPhy=new("BPhy",  this.full_name());
    ArPhy=new("ArPhy", this.full_name());
     RPhy=new("RPhy",  this.full_name());
    Collect=new("Collect",  this.full_name());
endfunction

virtual function void cfg(string mode="tvdump");
    AwPhy  .cfg("aw_put", mode, "nodrv");
     WPhy  .cfg(mode, "nodrv");
     BPhy  .cfg(1, mode, "nodrv" );
    ArPhy  .cfg("ar_put", mode, "nodrv");
     RPhy  .cfg(1, mode, "nodrv");
    Collect.cfg(mode);
endfunction

virtual function void connect(mem_trace_manager MemMan, 
                              virtual vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwPhyIf,
                              virtual vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1)  WPhyIf,
                              virtual vld_rdy_data_trace_interface#(2+IDW)  BPhyIf,
                              virtual vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2)  ArPhyIf,
                              virtual vld_rdy_data_trace_interface#(DATAW+IDW+3)  RPhyIf
                              );
    this.MemMan = MemMan;
    AwPhy.add_puts(Collect);
    AwPhy.add_puts(BPhy);
     WPhy.add_puts(Collect);
     BPhy.add_puts(Collect);

    ArPhy.add_puts(Collect);
    ArPhy.add_puts(RPhy);
     RPhy.add_puts(Collect);
    Collect.add_puts(this.MemMan);

    AwPhy.VldRdyDataIf = AwPhyIf;
     WPhy.VldRdyDataIf =  WPhyIf;
     BPhy.VldRdyDataIf =  BPhyIf;
    ArPhy.VldRdyDataIf = ArPhyIf;
     RPhy.VldRdyDataIf =  RPhyIf;
endfunction

virtual task run();
    fork
        AwPhy.run();
         WPhy.run();
         BPhy.run();
        ArPhy.run();
         RPhy.run();
        Collect.run();
    join
endtask

endclass

class maxi_trace_drvagent#(int ADDRW=64, int DATAW=64, int LENGTHW=8, int SIZEW=3, int IDW=1) extends trace_base_component;

axi_ctr_trace_phy_rdydrv#(ADDRW, LENGTHW, SIZEW, IDW) AwPhy;
axi_wdata_trace_phy_rdydrv#(DATAW, IDW) WPhy;
axi_brsp_trace_phy_vlddrv#(IDW) BPhy;
axi_ctr_trace_phy_rdydrv#(ADDRW, LENGTHW, SIZEW, IDW) ArPhy;
axi_rdata_trace_phy_vlddrv#(DATAW, IDW) RPhy;

axi_trace_collect Collect;

mem_trace_manager MemMan; //only one memory manager; shared by all maxi interface

function new(string inst_name, string hier_name);
    super.new(inst_name, hier_name);
    AwPhy=new("AwPhy", this.full_name());
     WPhy=new("WPhy",  this.full_name());
     BPhy=new("BPhy",  this.full_name());
    ArPhy=new("ArPhy", this.full_name());
     RPhy=new("RPhy",  this.full_name());
    Collect=new("Collect",  this.full_name());
endfunction

virtual function void cfg(string mode="tvread");
    AwPhy  .cfg("aw_put", mode, "rdydrv");
     WPhy  .cfg(mode, "rdydrv");
     BPhy  .cfg(mode, "vlddrv" );
    ArPhy  .cfg("ar_put", mode, "rdydrv");
     RPhy  .cfg(mode, "vlddrv");
    Collect.cfg(mode);
endfunction

virtual function void connect(mem_trace_manager MemMan, 
                              virtual vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwPhyIf,
                              virtual vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1)  WPhyIf,
                              virtual vld_rdy_data_trace_interface#(2+IDW)  BPhyIf,
                              virtual vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2)  ArPhyIf,
                              virtual vld_rdy_data_trace_interface#(DATAW+IDW+3)  RPhyIf
                              );
    this.MemMan = MemMan;
    AwPhy.add_puts(Collect);
    AwPhy.add_puts(BPhy);
     WPhy.add_puts(Collect);
     BPhy.add_puts(Collect);

    ArPhy.add_puts(Collect);
    ArPhy.add_puts(RPhy);
     RPhy.add_puts(Collect);
    Collect.add_puts(this.MemMan);

    AwPhy.VldRdyDataIf = AwPhyIf;
     WPhy.VldRdyDataIf =  WPhyIf;
     BPhy.VldRdyDataIf =  BPhyIf;
    ArPhy.VldRdyDataIf = ArPhyIf;
     RPhy.VldRdyDataIf =  RPhyIf;

    Collect.add_gets(this.MemMan);
    Collect.add_puts(RPhy);
    Collect.add_puts(BPhy);

endfunction

virtual task run();
    fork
        AwPhy.run();
         WPhy.run();
         BPhy.run();
        ArPhy.run();
         RPhy.run();
        Collect.run();
    join
endtask

endclass



`endif
