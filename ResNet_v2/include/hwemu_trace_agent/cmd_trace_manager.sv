`ifndef CMD_TRACE_MANAGER__SV
`define CMD_TRACE_MANAGER__SV
    
`define ISNAMELETTER(x)  (x>="a"&&x<="z"||x>="A"&&x<="Z"||x=="_")
`define ISDIGIT(x)       (x>="0"&&x<="9")
`define ISSPACE(x)       (x==" "||x=="\t"||x=="\n"||x==13||x==14)
`define EOF -1

class cmd_trace_manager extends trace_base_component;
    static bit CfgSta[$];
    static bit ResRdSta[$];

    virtual blk_ctrl_interface BlkCtrlIf;

    int CmdMId; //if blkctrl is bundled to axi, the port with such bundling would be CmdM to do BlkCtrl
                //if not, 0 port would be CmdM to do BlkCtrl
    logic[7:0] ResMem[*];

    string FileWrCmdPath;
    string FileRdCmdPath;
    string FileRdResPath;
    integer FpWrCmd;
    integer FpRdCmd;
    integer FpRdRes;
    int    LineNumWrCmd;
    int    LineNumRdCmd;
    int    LineNumRdRes;

    int    RdyCnt;
    int    DonCnt;
    int    WrCmdCnt;
    int    RdCmdCnt;
    int    TxWrCmdCnt;
    int    TxRdCmdCnt;

    bit IsBlkCtrlCmdM; //if this cmd_manager is the manager to do blkctrl
    string mode;
    bit IsAxiBlkCtrl;
    bit IsPipeline;

    function new(string inst_name, string hier_name);
        super.new(inst_name, hier_name);
    endfunction

    function void cfg(int cmd_manager_id, string mode, string filepre, bit isaxiblkctrl, bit isblkctrlcmdm, bit ispipeline);
        this.FileWrCmdPath   = $sformatf("%s/%s_wrcmd.dat",      this.TVDir, filepre);
        this.FileRdCmdPath   = $sformatf("%s/%s_rdcmd.dat",      this.TVDir, filepre);
        this.FileRdResPath   = $sformatf("%s/%s_rdresult.dat",   this.TVDir, filepre);
        this.mode = mode;
        this.IsBlkCtrlCmdM = isblkctrlcmdm;
        this.IsAxiBlkCtrl  = isaxiblkctrl;
        this.IsPipeline = ispipeline;
        this.CmdMId = cmd_manager_id;
        this.LineNumWrCmd = 0;
        this.LineNumRdCmd = 0;
        this.LineNumRdRes = 0;
        this.RdyCnt     = 0;
        this.DonCnt     = 0;
        this.WrCmdCnt     = 0;
        this.RdCmdCnt     = 0;
        this.TxWrCmdCnt     = 0;
        this.TxRdCmdCnt     = 0;
        this.ResMem.delete();
        this.CfgSta.push_back(0);
        this.ResRdSta.push_back(0);
        if(mode=="tvdump") begin
            FpWrCmd  = $fopen(this.FileWrCmdPath, "w");
            FpRdCmd  = $fopen(this.FileRdCmdPath, "w");
            FpRdRes  = $fopen(this.FileRdResPath, "w");
        end else if(mode=="tvread") begin
            FpWrCmd  = $fopen(this.FileWrCmdPath, "r");
            FpRdCmd  = $fopen(this.FileRdCmdPath, "r");
            FpRdRes  = $fopen(this.FileRdResPath, "r");
        end
    endfunction

    function void finish();
        $fclose(FpWrCmd);
        $fclose(FpRdCmd);
        $fclose(FpRdRes);
    endfunction


    virtual function void one_evt_dump(integer fp, string evt, int cnt);
        if(mode!="tvdump") return;
        $fdisplay(fp, "%s%0d", evt, cnt);
    endfunction

    virtual function void one_trans_result_dumpchk();
        int addr;
        string line;
        logic[7:0] data;
        int i;
        int cnt=0;
        if(mode=="tvdump") begin
            $fdisplay(FpRdRes, "__transaction_start%0d", this.DonCnt); this.LineNumRdRes++;
            if(ResMem.first(addr) && FpRdRes)
                do begin
                    $fdisplay(FpRdRes, "%0d %h", addr, ResMem[addr]); this.LineNumRdRes++;
                end while( ResMem.next(addr) );
            $fdisplay(FpRdRes, "__transaction_end%0d", this.DonCnt); this.LineNumRdRes++;
        end
        if(mode=="tvread") begin
            byte_mem_data chkdata = new;
            chkdata.Mem = ResMem;
            hook_data(chkdata, "axilite_output");
            while($fgets(line, FpRdRes)) begin
                this.LineNumRdRes++;
                i=matchstrchk(line, "__transaction_start");
                if(i==0) continue;
                i=matchstrchk(line, "__transaction_end");
                if(i==0) break;
                void'($sscanf(line, "%d %h", addr, data));
                if(!ResMem.exists(addr))
                    $display("COSIM TRACE CHECK WITH HWEMU ERROR, %s can't find dut item at addr:%0h", this.full_name(), addr);
                else if(ResMem[addr]!=data)
                    $display("COSIM TRACE CHECK WITH HWEMU ERROR, %s check item failed at addr:%0h, ref:%0h, dut:%0h", 
                            this.full_name(), addr, data, ResMem[addr]);
                ResMem.delete(addr);
                cnt++;
            end
            if(ResMem.first(addr)) begin
                do begin
                    $display("COSIM TRACE CHECK WITH HWEMU ERROR, %s can't find ref item at addr:%0h", this.full_name(), addr);
                    ResMem.delete(addr);
                end while( ResMem.next(addr) );
            end
            $display("For %0dth transaction, %d axilite memory data are checked with HW_EMU in %s", this.DonCnt, cnt, this.full_name());
        end
    endfunction


    virtual function void one_cmd_dump(integer fd, string filename, axi_trace_data tr, ref int cnt, ref int linenum);
        if(mode != "tvdump") begin cnt++; return; end
        $fwrite(fd, "addr%0d", cnt);
        foreach(tr.addr[i]) $fwrite(fd, " %0h", tr.addr[i]);
        $fwrite(fd, "\n");                                                linenum++;
        $fwrite(fd, "data%0d", cnt);
        foreach(tr.data[i]) $fwrite(fd, " %0h", tr.data[i]);
        $fwrite(fd, "\n");                                                linenum++;
        $fdisplay(fd, "iswr%0d %0h",  cnt, (tr.tr_type>1&&tr.tr_type<5) ); linenum++;
        $fdisplay(fd, "id%0d %0h",    cnt, tr.id );                        linenum++;
        $fdisplay(fd, "burst%0d %0h", cnt, tr.burst );                     linenum++;
        $fdisplay(fd, "len%0d %0h",   cnt, tr.len   );                     linenum++;
        $fdisplay(fd, "size%0d %0h",  cnt, tr.size  );                     linenum++;
        $fdisplay(fd, "" );                     linenum++;
        cnt++;
    endfunction

    virtual function string one_set_wrcmd_read();
        return one_sect_cmd_read(FpWrCmd, FileWrCmdPath, LineNumWrCmd, TxWrCmdCnt);
    endfunction

    virtual function string one_set_rdcmd_read();
        return one_sect_cmd_read(FpRdCmd, FileRdCmdPath, LineNumRdCmd, TxRdCmdCnt);
    endfunction

    virtual function int get_next_field(string ss);
        int i;
        int state=0;
        for(i=0;i<ss.len;i++) begin
            //find the first field
            if( state==0&& (ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="z"||ss[i]>="A"&&ss[i]<="Z"||ss[i]=="_") ) state=1;
            //continue to scan the first field
            if( state==1&& (ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="z"||ss[i]>="A"&&ss[i]<="Z"||ss[i]=="_") ) state=1;
            //scan the first field done
            if( state==1&&!(ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="z"||ss[i]>="A"&&ss[i]<="Z"||ss[i]=="_") ) state=2;
            //scan null letter after the first field
            if( state==2&&!(ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="z"||ss[i]>="A"&&ss[i]<="Z"||ss[i]=="_") ) state=2;
            //find the second field
            if( state==2&& (ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="z"||ss[i]>="A"&&ss[i]<="Z"||ss[i]=="_") ) return i;
        end
        return -1;
    endfunction

    function string one_sect_cmd_read(input integer fd, input string filename, ref int linenum, ref int cmdcnt);
        string line;
        int cntaddr, cntdata, cntiswr, cntid, cntburst, cntlen, cntsize;
        axi_trace_data tmptr = new();

        one_sect_cmd_read = "nodata_read";
        cntaddr=-2; cntdata=-2; cntiswr=-2; cntid=-2; cntburst=-2; cntlen =-2; cntsize=-2;
        while($fgets(line, fd)) begin
            logic[63:0] datas[$]; 
            string lineproc; 
            int i=matchstrchk(line, "//");

            linenum++;
            if(i==-1) lineproc = line;
            if(i==0) continue;
            if(i>1) lineproc = line.substr(0, i-1);
            i=matchstrchk(lineproc, "__trace_ap_ready_evt");
            if(i==0) return "wait_ap_ready";
            i=matchstrchk(lineproc, "__trace_ap_done_evt");
            if(i==0) return "wait_ap_done";
            
            void'($sscanf(lineproc, "addr%d", cntaddr));
            void'($sscanf(lineproc, "data%d", cntdata));
            void'($sscanf(lineproc, "iswr%d", cntiswr));
            void'($sscanf(lineproc, "id%d", cntid));
            void'($sscanf(lineproc, "burst%d", cntburst));
            void'($sscanf(lineproc, "len%d", cntlen));
            void'($sscanf(lineproc, "size%d", cntsize));
            if(cntaddr>=0||cntdata>=0||cntiswr>=0||cntid>=0||cntburst>=0||cntlen >=0||cntsize>=0) begin
                int aa;
                logic[63:0] tmp;
                aa = get_next_field(lineproc);
                lineproc = lineproc.substr(aa, lineproc.len-1);
                while( $sscanf(lineproc, "%h", tmp)!=`EOF ) begin
                    if(cntaddr >=0) begin tmptr.addr.push_back(tmp);   end
                    if(cntdata >=0) begin tmptr.data.push_back(tmp);   end
                    if(cntiswr >=0) begin tmptr.tr_type = 2* tmp   ;   end
                    if(cntid   >=0) begin tmptr.id = tmp           ;   end
                    if(cntburst>=0) begin tmptr.burst = tmp        ;   end
                    if(cntlen  >=0) begin tmptr.len = tmp          ;   end
                    if(cntsize >=0) begin tmptr.size = tmp         ;   end
                    aa = get_next_field(lineproc);
                    if(aa==-1) break;
                    lineproc = lineproc.substr(aa, lineproc.len-1);
                end
                if(cntaddr >=0) begin   cntaddr  = -1; end
                if(cntdata >=0) begin   cntdata  = -1; end
                if(cntiswr >=0) begin   cntiswr  = -1; end
                if(cntid   >=0) begin   cntid    = -1; end
                if(cntburst>=0) begin   cntburst = -1; end
                if(cntlen  >=0) begin   cntlen   = -1; end
                if(cntsize >=0) begin   cntsize  = -1; end
            end
            if(cntaddr==-1&&cntdata==-1&&cntiswr==-1&&cntid==-1&&cntburst==-1&&cntlen ==-1&&cntsize==-1) begin
                put_data(tmptr, tmptr.tr_type<2 ? "rdcmd" : "wrcmd");
                cmdcnt++;
                tmptr = new();
                cntaddr=-2; cntdata=-2; cntiswr=-2; cntid=-2; cntburst=-2; cntlen =-2; cntsize=-2;
            end
        end
    endfunction

    function int matchstrchk(string longstr, string str);
        matchstrchk = -1;
        for(int i=0; i<=longstr.len - str.len; i++)
            if(longstr.substr(i, i+str.len-1) == str) begin
                matchstrchk = i;
                break;
            end
    endfunction

    virtual function void feedback_rdata_postproc(axi_trace_data tr);
    endfunction

    virtual function void put_data_called(data_base_object data, string flag);
        axi_trace_data d_cast;
        if( !$cast(d_cast, data) ) $display("Error, data_base_object data can't be casted into axi_trace_data in %s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );
        case(flag)
            "axi_brx_end"  : begin 
                if(IsAxiBlkCtrl && IsBlkCtrlCmdM && d_cast.addr[0]==0 && (d_cast.data[0]&5'h10)) begin //write ap_continue
                    this.one_cmd_dump(FpRdCmd, FileRdCmdPath, d_cast, RdCmdCnt, LineNumRdCmd); 
                    //$display("lineww257, onecmdrx rd :%0d", RdCmdCnt);
                end else begin
                    this.one_cmd_dump(FpWrCmd, FileWrCmdPath, d_cast, WrCmdCnt, LineNumWrCmd); 
                    //$display("lineww260, onecmdrx wr :%0d", WrCmdCnt);
                end
            end "feedback_rdata" : begin 
                this.feedback_rdata_postproc(d_cast);
            end "axi_rrx_end" : begin 
                this.one_cmd_dump(FpRdCmd, FileRdCmdPath, d_cast, RdCmdCnt, LineNumRdCmd); 
                    //$display("lineww266, onecmdrx rd :%0d", RdCmdCnt);
                if( !(IsAxiBlkCtrl && IsBlkCtrlCmdM && d_cast.addr[0]==0) ) begin
                    foreach(d_cast.addr[i]) ResMem[d_cast.addr[i]] = d_cast.data[i];
                end
            end "aww_both_rx","aw_master","ar_master","w_master","b_master","ar_master","r_master" : begin
            end default : $display("undefined put_data_called flag:%s in %s at %t. TB file:%s line:%d", flag, this.full_name(), $time, `__FILE__, `__LINE__ );
        endcase
    endfunction

    virtual task dump_task();
        fork
            forever begin
                @(posedge BlkCtrlIf.clkrst_if.clk);
                //for write cmd, next transaction cmd must be executed after ap_ready
                //ap_ready can be polling in HW_EMU. But for cosim trace, ap_ready 
                //can be monitored by checking the signal directly
                #1ps;
                if(BlkCtrlIf.ap_ready===1&&BlkCtrlIf.ap_start===1)  begin
                    if(IsPipeline) one_evt_dump(FpWrCmd, "__trace_ap_ready_evt", this.RdyCnt);
                    this.RdyCnt++;
                end
            end
            forever begin
                @(posedge BlkCtrlIf.clkrst_if.clk);
                //for read cmd, polling cmd is in fact useless as ap_ready/ap_done/ap_idle
                //can be checked directly. Just think about the case that there is 
                //no polling cmd, only result read/ap_continue setting cmd are needed.
                //These cmds should be execute after ap_done.
                //For polling cmds, just insert them. These cmds would have problems if they
                //are blocked by ap_ready/ap_done events
                #1ps;
                if(BlkCtrlIf.ap_done===1&&BlkCtrlIf.ap_continue===1)  begin
                    if(DonCnt>0)  //for the first ap_done, result data won't be dumpped
                                  //as trace agent doesn't know when result reading is done
                        one_trans_result_dumpchk();
                    one_evt_dump(FpRdCmd, "__trace_ap_done_evt", this.DonCnt);
                    if(!IsPipeline) one_evt_dump(FpWrCmd, "__trace_ap_done_evt", this.DonCnt);
                    this.DonCnt++;
                    //$display("lineww299999999999999 %t", $realtime);
                end
                if(BlkCtrlIf.finish==1 && mode=="tvread") begin//finish can't be set for tvdump mode
                    one_trans_result_dumpchk();
                    wait(0);
                end
            end
       join
    endtask

    virtual task wrcmd_read_task();
        int totalcnt;
        if(mode!="tvread") wait(0);
        #2ps;
        totalcnt = BlkCtrlIf.ref_trans_cnt;
        forever begin
            string evt;
            evt = one_set_wrcmd_read();
            if(evt=="wait_ap_ready") begin
                totalcnt--;
                forever begin
                    @(posedge BlkCtrlIf.clkrst_if.clk);
                    #1ps;
                    if(BlkCtrlIf.ap_ready===1&&(BlkCtrlIf.ap_start===1&&totalcnt>0||totalcnt<=0) )  break;
                end
                $display("wrcmd push thread %0dth ap_ready waited", totalcnt);
            end
            if(evt=="wait_ap_done") begin
                totalcnt--;
                forever begin
                    @(posedge BlkCtrlIf.clkrst_if.clk);
                    #1ps;
                    if(BlkCtrlIf.ap_done===1&&(BlkCtrlIf.ap_continue===1&&totalcnt>0||totalcnt<=0) )  break;
                end
                $display("wrcmd push thread %0dth ap_done waited", totalcnt);
            end
            if(evt == "nodata_read") begin
                $display("wrcmd push thread:%0d wrcmd is pushed in %s at %t. TB file:%s line:%d", TxWrCmdCnt, this.full_name(), $time, `__FILE__, `__LINE__ );
                wait(WrCmdCnt==TxWrCmdCnt);
                CfgSta[CmdMId] = 1;
                wait(0);
            end
        end
    endtask

    virtual task rdcmd_read_task();
        int totalcnt;
        if(mode!="tvread") wait(0);
        #2ps;
        totalcnt = BlkCtrlIf.ref_trans_cnt;
        forever begin
            string evt;
            evt = one_set_rdcmd_read();
            if(evt=="wait_ap_ready") begin
                totalcnt--;
                forever begin
                    @(posedge BlkCtrlIf.clkrst_if.clk);
                    #1ps;
                    if(BlkCtrlIf.ap_ready===1&&(BlkCtrlIf.ap_start===1&&totalcnt>0||totalcnt<=0))  break;
                end
                $display("rdcmd push thread %0dth ap_ready waited", totalcnt);
            end
            if(evt=="wait_ap_done") begin
                totalcnt--;
                forever begin
                    @(posedge BlkCtrlIf.clkrst_if.clk);
                    #1ps;
                    if(BlkCtrlIf.ap_done===1&&(BlkCtrlIf.ap_continue===1&&totalcnt>0||totalcnt<=0))  break;
                end
                $display("rdcmd push thread %0dth ap_done waited", totalcnt);
            end
            if(evt == "nodata_read") begin
                $display("rdcmd push thread:%0d rdcmd is pushed in %s at %t. TB file:%s line:%d", TxRdCmdCnt, this.full_name(), $time, `__FILE__, `__LINE__ );
                wait(RdCmdCnt==TxRdCmdCnt);
                ResRdSta[CmdMId] = 1;
                wait(0);
            end
        end
    endtask

    virtual task blk_ctrl_task();
        if(IsBlkCtrlCmdM && mode=="tvread") begin
            int totalcnt;
            #2ps;
            totalcnt = BlkCtrlIf.ref_trans_cnt;
            fork
                forever begin
                    @(posedge BlkCtrlIf.clkrst_if.clk);
                    if(BlkCtrlIf.ap_done===1&&(BlkCtrlIf.ap_continue===1&&totalcnt>1||totalcnt==1)) totalcnt--;
                    if(totalcnt==0) begin
                        $display("all transaction processing dones are monitored");
                        break;
                    end
                end
                begin
                    wait(CfgSta.size == CfgSta.sum() && ResRdSta.size==ResRdSta.sum() );
                    $display("all axilite interface cmds are executed");
                end
            join
            @(posedge BlkCtrlIf.clkrst_if.clk);
            BlkCtrlIf.finish = 1;
            @(posedge BlkCtrlIf.clkrst_if.clk);
            BlkCtrlIf.finish = 0;
            repeat(3) @(posedge BlkCtrlIf.clkrst_if.clk);
            this.finish();
            repeat(3) @(posedge BlkCtrlIf.clkrst_if.clk);
            $display("COSIM BASED HWEMU TRACE SIMULATION is finished");
            $finish;
        end else wait(0);
    endtask
 
    virtual task run();
        fork
            forever begin
                wait(BlkCtrlIf.clkrst_if.rst===1);
                fork
                    blk_ctrl_task();
                    dump_task();
                    wrcmd_read_task();
                    rdcmd_read_task();
                    begin
                        wait(BlkCtrlIf.clkrst_if.rst===0);
                    end
                join_any
                disable fork;
            end
        join
    endtask
endclass
`endif
