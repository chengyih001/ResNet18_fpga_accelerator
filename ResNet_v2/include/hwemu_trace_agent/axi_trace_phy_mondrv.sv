`ifndef AXI_TRACE_PHY_MONDRV__SV
`define AXI_TRACE_PHY_MONDRV__SV

`define DRV_DELAY 0ps
class axi_trace_data extends data_base_object;
    rand bit                             addr_done;
    rand bit                             data_done;
    rand bit                             brsp_done;

    rand int                             tr_type; //0:AR,1:R, 2:AW, 3:W, 4:B
    //rand int                             unrx_num;
    rand int                             bus_bytenum;

    rand int                             id;
    rand logic [7:0]                     data[$];
    rand longint                         addr[$];
    rand int                             len;
    rand int                             size;
    rand int                             burst; //0:fix, 1:incr, 2:wrap, 3:reserved
    //rand int                             lock;
    //rand int                             region;
    //rand int                             cache;
    //rand int                             prot;
    //rand int                             qos;
    //rand int                             user;
    rand int                             bresp;
    rand int                             rresp[$];
    rand int                             strb[$];
    rand bit                             last;
    longint                              clkcnt_sample;
    

    function new ();
        init();
    endfunction

    virtual function void init();
        addr_done       =  0;
        data_done       =  0;
        brsp_done       =  0;
        tr_type         =  0;
        bus_bytenum     = -1;

        id              =  0;
        len             =  0;//interface len+1
        size            =  0;
        burst           =  0;

        bresp           =  0;
        rresp.delete ;
        strb .delete ;
        addr .delete ;
        data .delete ;
        last            = 0;
        //unrx_num = 0;
        //lock     = NORMAL;
        //region   = 0;
        //cache    = WA0_RA0_C0_B0;
        //prot     = 0;
        //qos      = 0;
    endfunction

    virtual function axi_trace_data clone();
        clone = new();
        clone.addr_done       = this.addr_done    ;
        clone.data_done       = this.data_done    ;
        clone.brsp_done       = this.brsp_done    ;
        clone.tr_type         = this.tr_type      ;
        clone.bus_bytenum     = this.bus_bytenum  ;
        clone.id              = this.id           ;
        clone.len             = this.len          ; 
        clone.size            = this.size         ;
        clone.burst           = this.burst        ;
        clone.bresp           = this.bresp        ;
        clone.rresp           = this.rresp        ;
        clone.strb            = this.strb         ;
        clone.addr            = this.addr         ;
        clone.data            = this.data         ;
        clone.last            = this.last         ;
    endfunction

    virtual function string sprint();
        sprint = $sformatf("addr:%0h, tr_type:%0h, len:%0h, burst:%0h,size:%0h, last:%0h, bresp:%0h, bus_byte:%0h\ndata: ",
                            addr[0], tr_type, len, burst, size, last, bresp, bus_bytenum);
        foreach(data[i]) sprint = {sprint, $sformatf("%0h ", data[i])};
        sprint = {sprint,"\nstrb: "};
        foreach(strb[i]) sprint = {sprint, $sformatf("%0h ", strb[i])};
        sprint = {sprint,"\n"};
    endfunction

    virtual function void combine_ainfo(axi_trace_data tr);
        this.addr    = tr.addr    ; //this.addr must be none queue
        //this.region  = tr.region  ;
        this.len     = tr.len     ;
        this.size    = tr.size    ;
        this.burst   = tr.burst   ;
        //this.lock    = tr.lock    ;
        //this.cache   = tr.cache   ;
        //this.prot    = tr.prot    ;
        //this.qos     = tr.qos     ;
        this.addr_done = 1;
    endfunction

    function void combine_binfo(axi_trace_data tr);
        this.bresp     = tr.bresp;
    endfunction

    function void combine_dinfo(axi_trace_data tr);
        this.bus_bytenum = tr.bus_bytenum;
        this.data =  {this.data, tr.data};
        this.rresp = {this.rresp, tr.rresp};
        this.strb  = {this.strb,  tr.strb };
        this.last = tr.last;
    endfunction

    function void waddrdata_strobe_infer();
        int bytenum = 1<<size;
        longint tmpaddr;
        longint addrhigh;
        int dataidx;
        if(this.last&&this.tr_type>1&&this.tr_type<5) begin
            if(this.strb.size!=this.len) $display("error: this is last data of axi w transfer, data length can't match with aw specified length");
            tmpaddr = (this.addr[0]/this.bus_bytenum)*this.bus_bytenum;
            addrhigh = (this.addr[0]/bytenum)*bytenum + bytenum;
            dataidx=0;
            this.addr.delete();
            for(int j=0; j<this.strb.size; j++) begin
                for(int i=0; i<this.bus_bytenum; i++) begin
                    if( ((1<<i)&this.strb[j])>0 ) begin
                        this.addr.push_back(tmpaddr);
                        dataidx++;
                    end else this.data.delete(dataidx);
                    tmpaddr++;
                end
                tmpaddr = addrhigh / this.bus_bytenum * this.bus_bytenum;
                addrhigh += bytenum;
            end
        end
    endfunction

    function void raddrdata_infer();
        int bytenum = 1<<size;
        longint addrcnt;
        longint addrlow;
        int idx=0;
        if(this.last&&this.tr_type>=0&&this.tr_type<2) begin
            if(this.rresp.size!=this.len) $display("error: this is last data of axi r transfer, data length can't match with ar specified length");
            addrcnt = this.addr[0]/this.bus_bytenum*this.bus_bytenum;
            addrlow = this.addr[0];
            this.addr.delete();
            for(int j=0; j<this.len; j++) begin
                for(int i=0; i<this.bus_bytenum; i++) begin
            //$display("infer ongoing addrlow:%0h", addrlow);
                    if(addrcnt>=addrlow && addrcnt<addrlow/bytenum*bytenum+bytenum) begin
                        idx++;
                        this.addr.push_back(addrcnt);
                    end else this.data.delete(idx);
                    addrcnt++;
                end
                addrlow = addrlow/bytenum*bytenum+bytenum;
                addrcnt = addrlow/this.bus_bytenum*this.bus_bytenum;
            end
        end
    endfunction

endclass

class axi_ctr_trace_phy_mondrv#(int ADDRW=64, int LENGTHW=8, int SIZEW=3, int IDW=1) extends vld_rdy_data_trace_phy_mondrv#( (ADDRW+LENGTHW+SIZEW+IDW+2) );
    axi_trace_data PutDataAxi;
    string PutFlag; //only two value: aw_puts, ar_puts

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    //interface should follow these field sequence
    function void cfg(string PutFlag, string mode="tvdump", string drvmode="nodrv");
        super.cfg(mode, drvmode);
        //this.mode = mode;
        //this.drvmode = drvmode;
        this.PutFlag = PutFlag;
        DataField.push_back("address");
        DataBitWidth.push_back(ADDRW);
        DataField.push_back("burst");
        DataBitWidth.push_back(2);
        DataField.push_back("length");
        DataBitWidth.push_back(LENGTHW);
        DataField.push_back("size");
        DataBitWidth.push_back(SIZEW);
        DataField.push_back("id");
        DataBitWidth.push_back(IDW);
        PutDataAxi = new();
    endfunction

    virtual function void put_data(data_base_object data, string flag);
        super.put_data(this.PutData, PutFlag);
    endfunction

    virtual function void abstract_put_data();
        PutDataAxi.init();
        if(this.PutFlag == "aw_put")      PutDataAxi.tr_type =2;
        else if(this.PutFlag == "ar_put") PutDataAxi.tr_type =0;
        else begin
            $display("FATAL ERROR: %s PutFlag:%s is not right", this.full_name(), this.PutFlag);
            $finish;
        end
        
//$display("lineww, addr:%0h, len:%0h, size:%0h,  %s at %t. TB file:%s line:%d",PutDataProc.get_fielddata("address"), PutDataProc.get_fielddata("length"), PutDataProc.get_fielddata("size"), this.full_name(), $time, `__FILE__, `__LINE__ );
        PutDataAxi.addr[0] = PutDataProc.get_fielddata("address");
        PutDataAxi.burst = PutDataProc.get_fielddata("burst");
        PutDataAxi.len  = PutDataProc.get_fielddata("length")+1;
        PutDataAxi.size = PutDataProc.get_fielddata("size");
        PutDataAxi.id = PutDataProc.get_fielddata("id");
        PutDataAxi.clkcnt_sample = VldRdyDataIf.clkcnt;
        this.PutData = PutDataAxi;
    endfunction
endclass

class axi_rdata_trace_phy_mondrv#(int DATAW=64, int IDW=1) extends vld_rdy_data_trace_phy_mondrv#( (DATAW+IDW+3) );
    axi_trace_data PutDataAxi;
    bit IsArevtNeed;

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(bit is_arevt_need=1, string mode="tvdump", string drvmode="nodrv");
        super.cfg(mode, drvmode);
        //this.mode = mode;
        //this.drvmode = drvmode;
        DataField.push_back("data");
        DataBitWidth.push_back(DATAW);
        DataField.push_back("rresp");
        DataBitWidth.push_back(2);
        DataField.push_back("id");
        DataBitWidth.push_back(IDW);
        DataField.push_back("last");
        DataBitWidth.push_back(1);
        IsArevtNeed = is_arevt_need;
        if(is_arevt_need) EvtField.push_back("arevt"); 
        PutDataAxi = new();
    endfunction

    virtual function void abstract_put_data();
        logic[DATAW-1:0] data;
        PutDataAxi.init();
        PutDataAxi.tr_type =1;
        PutDataAxi.bus_bytenum = DATAW/8;
        data             = PutDataProc.get_fielddata("data");
        for(int i=0; i<DATAW/8; i++) begin
            PutDataAxi.data.push_back(data&8'hff);
            data = data>>8;
        end
        PutDataAxi.rresp[0] = PutDataProc.get_fielddata("rresp");
        PutDataAxi.id    = PutDataProc.get_fielddata("id");
        PutDataAxi.last  = PutDataProc.get_fielddata("last");
        PutDataAxi.clkcnt_sample = VldRdyDataIf.clkcnt;
        this.PutData = PutDataAxi;
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        case(flag)
            "ar_put" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                if(!IsArevtNeed) return;
                for(int i=0; i<d_cast.len; i++) begin
                    ClkCyc["arevt"].push_back(i==0 ? d_cast.clkcnt_sample : -1);
//if(i==0)
//$display("lineww, arevt_clkcnt:%h %s at %t. TB file:%s line:%d", ClkCyc["arevt"][$], this.full_name(), $time, `__FILE__, `__LINE__ );
                end
            end "feedback_rdata" : begin
            end
        endcase
    endfunction


    virtual function void put_data(data_base_object data, string flag);
        super.put_data(this.PutData, "r_put");
    endfunction
endclass

class axi_wdata_trace_phy_mondrv#(int DATAW=64, int IDW=1) extends vld_rdy_data_trace_phy_mondrv#( (DATAW+DATAW/8+IDW+1) );
    axi_trace_data PutDataAxi;

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string mode="tvdump", string drvmode="nodrv");
        super.cfg(mode, drvmode);
        //this.mode = mode;
        //this.drvmode = drvmode;
        DataField.push_back("data");
        DataBitWidth.push_back(DATAW);
        DataField.push_back("strobe");
        DataBitWidth.push_back(DATAW/8);
        DataField.push_back("id");
        DataBitWidth.push_back(IDW);
        DataField.push_back("last");
        DataBitWidth.push_back(1);
        PutDataAxi = new();
    endfunction

    virtual function void abstract_put_data();
        logic[DATAW-1:0] data;
        PutDataAxi.init();
        PutDataAxi.bus_bytenum = DATAW/8;
        PutDataAxi.tr_type =3;
        data             = PutDataProc.get_fielddata("data");
        for(int i=0; i<DATAW/8; i++) begin
            PutDataAxi.data.push_back(data&8'hff);
            data = data>>8;
        end
        PutDataAxi.strb[0] = PutDataProc.get_fielddata("strobe");
        PutDataAxi.id    = PutDataProc.get_fielddata("id");
        PutDataAxi.last  = PutDataProc.get_fielddata("last");
        PutDataAxi.clkcnt_sample = VldRdyDataIf.clkcnt;
        this.PutData = PutDataAxi;
    endfunction

    virtual function void put_data(data_base_object data, string flag);
        super.put_data(this.PutData, "w_put");
    endfunction

endclass

class axi_brsp_trace_phy_mondrv#(int IDW=1) extends vld_rdy_data_trace_phy_mondrv#( 2+IDW );
    axi_trace_data PutDataAxi;
    bit IsAwevtNeed;

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(bit isawevt_need=1, string mode="tvdump", string drvmode="nodrv" );
        super.cfg(mode, drvmode);
        //this.mode = mode;
        //this.drvmode = drvmode;
        this.IsAwevtNeed = isawevt_need;
        DataField.push_back("bresp");
        DataBitWidth.push_back(2);
        DataField.push_back("id");
        DataBitWidth.push_back(IDW);
        if(isawevt_need) EvtField.push_back("awevt");
        PutDataAxi = new();
    endfunction

    virtual function void abstract_put_data();
        logic[DATAW-1:0] data;
        PutDataAxi.init();
        PutDataAxi.tr_type =4;
        PutDataAxi.bresp = PutDataProc.get_fielddata("bresp");
        PutDataAxi.clkcnt_sample = VldRdyDataIf.clkcnt;
        this.PutData = PutDataAxi;
    endfunction

    virtual function void put_data(data_base_object data, string flag);
//$display("lineww, in b_phymon put_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
        super.put_data(this.PutData, "b_put");
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        case(flag)
            "aw_put" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                if(IsAwevtNeed) begin
                    ClkCyc["awevt"].push_back(d_cast.clkcnt_sample);
//$display("lineww, awevt_clkcnt:%h %s at %t. TB file:%s line:%d", ClkCyc["awevt"][$], this.full_name(), $time, `__FILE__, `__LINE__ );
                end
            end "b_master": begin
            end default: begin
            end
        endcase
    endfunction
endclass

//////////////////////////////////////////////////////
/////////////        driver part       ///////////////
//////////////////////////////////////////////////////
class axi_ctr_trace_phy_vlddrv#(int ADDRW=64, int LENGTHW=8, int SIZEW=3, int IDW=1) extends axi_ctr_trace_phy_mondrv#(ADDRW, LENGTHW, SIZEW, IDW);
    logic[ADDRW+LENGTHW+SIZEW+IDW+1:0] HighTxData[$];

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string PutFlag, string mode="tvread", string drvmode="vlddrv");
        super.cfg(PutFlag, mode, drvmode);
    endfunction

    virtual function void master_atr2phydata(axi_trace_data tr); 
        logic[ADDRW-1:0] addr = tr.addr[0];
        logic[1:0] burst      = tr.burst;
        logic[LENGTHW-1:0] len = tr.len - 1;
        logic[SIZEW-1:0] size = tr.size;
        logic[IDW-1:0] id = tr.id;
        HighTxData.push_back({id, size, len, burst, addr});
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        case(flag)
            "aw_master": begin
                if(this.PutFlag=="ar_put") return;
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                master_atr2phydata(d_cast);
            end "ar_master": begin
                if(this.PutFlag=="aw_put") return;
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                master_atr2phydata(d_cast);
            end default : begin end
        endcase
    endfunction

    virtual task vlddrv_task();
        int mismat_rdycnt=0;
        int mismat_vldcnt=0;
        longint sum_vldgap = 0;
        longint cnt_vldgap = 0;
        int clkcnt_cmp;
        longint prevld_clkcnt;
        VldRdyDataIf.valid = 0;
        forever begin
            @(posedge VldRdyDataIf.clk);
            clkcnt_cmp=0;
            if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===1) begin
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.clkcnt<RefClkCyc["rdyevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                //No data check as we assume ar/aw vld drive is controlled by host, data should be no change
                if(RefData.size>1) begin
                    sum_vldgap += RefClkCyc["vldevt"][1] - RefClkCyc["vldevt"][0];
                    cnt_vldgap++;
                end
                RefData.pop_front();
                HighTxData.pop_front();
                foreach(EvtField[i]) RefClkCyc[EvtField[i]].pop_front();
                #`DRV_DELAY; VldRdyDataIf.valid = 0;
            end
            if(HighTxData.size==0) begin
                @(negedge VldRdyDataIf.clk);
                clkcnt_cmp = 1;
            end
            if( HighTxData.size>0 && (RefData.size>0  && VldRdyDataIf.clkcnt-clkcnt_cmp>=RefClkCyc["vldevt"][0]-1 ||
                  RefData.size==0 && cnt_vldgap>0 && VldRdyDataIf.clkcnt-clkcnt_cmp>=prevld_clkcnt+sum_vldgap/cnt_vldgap||
                                     RefData.size==0 && cnt_vldgap==0) ) begin
                //if(RefData.size>0&&mismat_vldcnt<5 && VldRdyDataIf.clkcnt-clkcnt_cmp!=RefClkCyc["vldevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut vld timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                //if(RefData.size==0&&mismat_vldcnt<5) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects high level has more data to send at %t, compared with HW_EMU PhyTV. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                prevld_clkcnt = VldRdyDataIf.clkcnt;
                #`DRV_DELAY; VldRdyDataIf.valid = 1;
                VldRdyDataIf.data = HighTxData[0];
            end
        end
    endtask
endclass

class axi_ctr_trace_phy_rdydrv#(int ADDRW=64, int LENGTHW=8, int SIZEW=3, int IDW=1) extends axi_ctr_trace_phy_mondrv#(ADDRW, LENGTHW, SIZEW, IDW);
    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction
    function void cfg(string PutFlag, string mode="tvread", string drvmode="rdydrv");
        super.cfg(PutFlag, mode, drvmode);
    endfunction
endclass


class axi_rdata_trace_phy_rdydrv#(int DATAW=64, int IDW=1) extends axi_rdata_trace_phy_mondrv#(DATAW, IDW);
    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction
    function void cfg(string mode="tvread", string drvmode="rdydrv");
        super.cfg(0, mode, drvmode);
    endfunction
    //high level data is useless as dut give rdata rsp after ar cmd which is sent by high level
    //rdata will be check at cmd trace manager
endclass

class axi_rdata_trace_phy_vlddrv#(int DATAW=64, int IDW=1) extends axi_rdata_trace_phy_mondrv#(DATAW, IDW);
    logic[DATAW+IDW+2:0] HighTxData[$];
    bit[1:0]             HighTxSta[$];
    longint              ArClkCyc[$];
    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string mode="tvread", string drvmode="vlddrv");
        super.cfg(1, mode, drvmode);
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        super.put_data_called(data, flag);
        case(flag)
            "ar_put" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                if(!IsArevtNeed) return;
                for(int i=0; i<d_cast.len; i++) 
                    ArClkCyc.push_back(i==0 ? d_cast.clkcnt_sample : -1);
            end "feedback_rdata" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                feedback_rtr2phydata(d_cast);
            end
        endcase
    endfunction

    virtual function void feedback_rtr2phydata(axi_trace_data tr); 
        int bus_bytenum = DATAW/8;
        int bytenum = 1<<tr.size;
        longint scan_addr = tr.addr[0]/bus_bytenum * bus_bytenum;
        longint addrhigh  = tr.addr[0]/bytenum*bytenum+bytenum;
        int idx=0;
        logic[IDW-1:0] id = tr.id; 
        logic[1:0] resp = 0;
        logic last;
        for(int i=0; i<tr.len; i++) begin
            logic[DATAW-1:0] data = 0;
            for(int j=0; j<bus_bytenum; j++) begin
                if(scan_addr == tr.addr[idx]&&scan_addr<addrhigh) data[j*8+7 -: 8] = tr.data[idx++];
                else data[j*8+7 -: 8] = 'hz;
                scan_addr++;
            end
            scan_addr = addrhigh / bus_bytenum*bus_bytenum;
            addrhigh += bytenum;
            if(i==tr.len-1) last = 1;
            else            last = 0;
            HighTxData.push_back({last, id, resp, data});
            if(i==0) HighTxSta.push_back(0);
            else if(i==tr.len-1) HighTxSta.push_back(2);
            else HighTxSta.push_back(1);
        end
    endfunction

    virtual task vlddrv_task();
        int mismat_rdycnt=0;
        int mismat_vldcnt=0;
        longint sum_vldgap = 0;
        longint cnt_vldgap = 0;
        longint sum_1stdelay = 0;
        longint cnt_1sttr = 0;
        int cnt_cmp;
        longint prevld_clkcnt;
        longint prevld_refclkcnt;
        bit chk;
        VldRdyDataIf.valid = 0;
        forever begin
            @(posedge VldRdyDataIf.clk);
            cnt_cmp=0;
            if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===1) begin
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.clkcnt<RefClkCyc["rdyevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                ////rdata is drived by TB. But rdata may be fetched from different address compared with HW_EMU,
                ////as previously DUT may send different address read request
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.data!=RefData[0]) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut output data mismatch at %t, compared with HW_EMU Phy. TB file:%s line:%d. Ref:0x%0h, Dut:0x%0h", 
                //         this.full_name(), $time, `__FILE__, `__LINE__, RefData[0], VldRdyDataIf.data);
                //    mismat_rdycnt++;
                //end
                if(RefData.size>1&&RefClkCyc["arevt"][1]==-1) begin
                    sum_vldgap += RefClkCyc["vldevt"][1] - RefClkCyc["vldevt"][0];
                    cnt_vldgap++;
                end
                if(RefData.size>0&&RefClkCyc["arevt"][0]>0) begin
                    sum_1stdelay += RefClkCyc["vldevt"][0] - RefClkCyc["arevt"][0];
                    cnt_1sttr++;
                end

                if(RefData.size>0) prevld_refclkcnt = RefClkCyc["vldevt"][0];

                if(HighTxSta[0]==0&&RefData.size>0&&RefClkCyc["arevt"][0]>  0||
                   HighTxSta[0]> 0&&RefData.size>0&&RefClkCyc["arevt"][0]==-1) begin
                    void'(RefData.pop_front()   );
                    foreach(EvtField[i]) void'(RefClkCyc[EvtField[i]].pop_front());
                end
                void'(HighTxData.pop_front());
                void'(HighTxSta.pop_front() );
                void'(ArClkCyc.pop_front()  );

                chk=0;
                while(HighTxSta[0]==2&&RefData.size>1&&RefClkCyc["arevt"][1]==-1||
                      HighTxSta[0]==0&&RefData.size>1&&RefClkCyc["arevt"][0]==-1) begin
                    chk =1;
                    void'(RefData.pop_front()   );
                    foreach(EvtField[i]) void'(RefClkCyc[EvtField[i]].pop_front());
                end
                //if(chk && mismat_vldcnt<5) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut trans len mismatch at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                #`DRV_DELAY; VldRdyDataIf.valid = 0;
            end
            if(HighTxData.size==0) begin 
                @(negedge VldRdyDataIf.clk);
                cnt_cmp = 1;
            end
            if(   (  HighTxData.size>0 && HighTxSta[0]==0 &&
                     ( RefData.size>0 && RefClkCyc["arevt"][0]>0 && VldRdyDataIf.clkcnt-cnt_cmp>=ArClkCyc[0]+RefClkCyc["vldevt"][0]-RefClkCyc["arevt"][0]-1||
                       (RefData.size==0||RefClkCyc["arevt"][0]==-1) && cnt_1sttr>0 && VldRdyDataIf.clkcnt-cnt_cmp>=ArClkCyc[0]+sum_1stdelay/cnt_1sttr        ||
                       (RefData.size==0||RefClkCyc["arevt"][0]==-1) &&cnt_1sttr==0 )  ) ||
                  (  HighTxData.size>0 && HighTxSta[0]>0 &&
                     ( RefData.size>0 && RefClkCyc["arevt"][0]==-1&&VldRdyDataIf.clkcnt>=prevld_clkcnt+RefClkCyc["vldevt"][0]-prevld_refclkcnt-1||
                       (RefData.size==0||RefClkCyc["arevt"][0]>0) && cnt_vldgap>0 && VldRdyDataIf.clkcnt>=prevld_clkcnt+sum_vldgap/cnt_vldgap  ||
                       (RefData.size==0||RefClkCyc["arevt"][0]>0) && cnt_vldgap==0 )  )   ) begin

                //if(HighTxSta[0]==0&&RefData.size>0&&RefClkCyc["arevt"][0]>  0&&mismat_vldcnt<5&&VldRdyDataIf.clkcnt-cnt_cmp!=RefClkCyc["vldevt"][0]-1||
                //   HighTxSta[0]> 0&&RefData.size>0&&RefClkCyc["arevt"][0]==-1&&mismat_vldcnt<5&&VldRdyDataIf.clkcnt-cnt_cmp!=RefClkCyc["vldevt"][0]-1 ) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                //if(HighTxSta[0]==0&&(RefData.size==0||RefClkCyc["arevt"][0]==-1)&&mismat_vldcnt<5 ||
                //   HighTxSta[0]> 0&&(RefData.size==0||RefClkCyc["arevt"][0]>  0)&&mismat_vldcnt<5) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects high level transaction is totally different at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                prevld_clkcnt = VldRdyDataIf.clkcnt;
                #`DRV_DELAY; VldRdyDataIf.valid = 1;
                VldRdyDataIf.data = HighTxData[0];
            end
        end
    endtask
endclass

class axi_wdata_trace_phy_vlddrv#(int DATAW=64, int IDW=1) extends axi_wdata_trace_phy_mondrv#(DATAW, IDW);
    logic[DATAW+DATAW/8+IDW:0] HighTxData[$];

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string mode="tvread", string drvmode="vlddrv");
        super.cfg(mode, drvmode);
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        case(flag)
            "w_master" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                master_wtr2phydata(d_cast);
            end default : begin end
        endcase
    endfunction

    virtual function void master_wtr2phydata(axi_trace_data tr); 
        int bus_bytenum = DATAW/8;
        int bytenum = 1<<tr.size;
        longint scan_addr = tr.addr[0]/bus_bytenum * bus_bytenum;
        longint addrhigh = tr.addr[0]/bytenum * bytenum + bytenum;
        int idx=0;
        logic[IDW-1:0] id = tr.id; 
        logic last;
        for(int i=0; i<tr.len; i++) begin
            logic[DATAW-1:0] data = 0;
            logic[DATAW/8-1:0] strb = 0;
            for(int j=0; j<bus_bytenum; j++) begin
                if(scan_addr == tr.addr[idx]) begin
                    data[j*8+7 -: 8] = tr.data[idx++];
                    strb[j] = 1;
                end else begin
                    data[j*8+7 -: 8] = 'hz;
                    strb[j] = 0;
                end
                scan_addr++;
            end
            scan_addr = addrhigh / bus_bytenum * bus_bytenum;
            addrhigh += bytenum;
            if(i==tr.len-1) last = 1;
            else            last = 0;
            HighTxData.push_back({last, id, strb, data});
        end
    endfunction

    virtual task vlddrv_task();
        int mismat_rdycnt=0;
        int mismat_vldcnt=0;
        longint sum_vldgap = 0;
        longint cnt_vldgap = 0;
        int clkcnt_cmp;
        longint prevld_clkcnt;
        VldRdyDataIf.valid = 0;
        forever begin
            @(posedge VldRdyDataIf.clk);
            clkcnt_cmp=0;
            if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===1) begin
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.clkcnt<RefClkCyc["rdyevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                //No data check as we assume ar/aw vld drive is controlled by host, should be no change or with a intented change
                if(RefData.size>1) begin
                    sum_vldgap += RefClkCyc["vldevt"][1] - RefClkCyc["vldevt"][0];
                    cnt_vldgap++;
                end
                RefData.pop_front();
                HighTxData.pop_front();
                foreach(EvtField[i]) RefClkCyc[EvtField[i]].pop_front();
                #`DRV_DELAY; VldRdyDataIf.valid = 0;
            end
            if(HighTxData.size==0) begin
                @(negedge VldRdyDataIf.clk);
                clkcnt_cmp = 1;
            end
            if( HighTxData.size>0 && (RefData.size>0  && VldRdyDataIf.clkcnt-clkcnt_cmp>=RefClkCyc["vldevt"][0]-1 ||
                  RefData.size==0 && cnt_vldgap>0 && VldRdyDataIf.clkcnt-clkcnt_cmp>=prevld_clkcnt+sum_vldgap/cnt_vldgap||
                                     RefData.size==0 && cnt_vldgap==0) ) begin
                //if(RefData.size>0&&mismat_vldcnt<5 && VldRdyDataIf.clkcnt-clkcnt_cmp!=RefClkCyc["vldevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut vld timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                //if(RefData.size==0&&mismat_vldcnt<5) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects high level has more data to send at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                prevld_clkcnt = VldRdyDataIf.clkcnt;
                #`DRV_DELAY; VldRdyDataIf.valid = 1;
                VldRdyDataIf.data = HighTxData[0];
            end
        end
    endtask
endclass

class axi_wdata_trace_phy_rdydrv#(int DATAW=64, int IDW=1) extends axi_wdata_trace_phy_mondrv#(DATAW, IDW);
    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string mode="tvread", string drvmode="rdydrv");
        super.cfg(mode, drvmode);
    endfunction
endclass

class axi_brsp_trace_phy_rdydrv#(int IDW=1)  extends axi_brsp_trace_phy_mondrv#(IDW);
    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string mode="tvread", string drvmode="rdydrv" );
        super.cfg(0, mode, drvmode);
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        super.put_data_called(data, flag);
        case(flag)
            "aw_put" : begin
            end "b_master": begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                master_btr2phydata(d_cast);
            end default: begin
            end
        endcase
    endfunction

    virtual function void master_btr2phydata(axi_trace_data tr); 
        //logic[IDW-1:0] id = tr.id; 
        //logic[1:0] resp;
        //HighTxData.push_back({id,resp});
    endfunction
endclass

class axi_brsp_trace_phy_vlddrv#(int IDW=1)  extends axi_brsp_trace_phy_mondrv#(IDW);
    longint          AwClkCyc[$];
    logic[IDW+1:0]   HighTxData[$];
    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(string mode="tvread", string drvmode="rdydrv" );
        super.cfg(1, mode, drvmode);
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        logic[IDW-1:0] id;
        super.put_data_called(data, flag);
        case(flag)
            "aw_put" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                if(IsAwevtNeed) AwClkCyc.push_back(d_cast.clkcnt_sample);
            end "aww_both_rx" : begin
                if( !$cast(d_cast, data) ) 
                    $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
                id = d_cast.id;
                HighTxData.push_back({id,2'b0});
//$display("lineww, aww_both_rx in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
            end default: begin
            end
        endcase
    endfunction

    virtual task vlddrv_task();
        int mismat_rdycnt=0;
        int mismat_vldcnt=0;
        longint sum_delay = 0;
        longint cnt_tr = 0;
        int clkcnt_cmp;
        VldRdyDataIf.valid = 0;
        forever begin
            @(posedge VldRdyDataIf.clk);
            clkcnt_cmp=0;
            if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===1) begin
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.clkcnt<RefClkCyc["rdyevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                if(RefData.size>0) begin
                    sum_delay += RefClkCyc["vldevt"][0] - RefClkCyc["awevt"][0];
                    cnt_tr++;
                end
                void'(HighTxData.pop_front());
                void'(AwClkCyc.pop_front()  );
                void'(RefData.pop_front()   );
                foreach(EvtField[i]) void'(RefClkCyc[EvtField[i]].pop_front());
                #`DRV_DELAY; VldRdyDataIf.valid = 0;
            end
            if(HighTxData.size==0) begin
                @(negedge VldRdyDataIf.clk);
                clkcnt_cmp = 1;
            end
            if(  HighTxData.size>0 && 
                 (RefData.size> 0&&VldRdyDataIf.clkcnt-clkcnt_cmp>=AwClkCyc[0]+RefClkCyc["vldevt"][0]-RefClkCyc["awevt"][0]-1||
                  RefData.size==0&&cnt_tr>0&&VldRdyDataIf.clkcnt-clkcnt_cmp>=AwClkCyc[0]+sum_delay/cnt_tr||
                  RefData.size==0&&cnt_tr==0)  ) begin

                //if(RefData.size>0&&mismat_vldcnt<5&&VldRdyDataIf.clkcnt-clkcnt_cmp!=AwClkCyc[0]+RefClkCyc["vldevt"][0]-RefClkCyc["awevt"][0]) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                //if(RefData.size==0&&mismat_vldcnt<5) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects high level transaction is totally different at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                #`DRV_DELAY; VldRdyDataIf.valid = 1;
                VldRdyDataIf.data = HighTxData[0];
            end
        end
    endtask
endclass

`endif
