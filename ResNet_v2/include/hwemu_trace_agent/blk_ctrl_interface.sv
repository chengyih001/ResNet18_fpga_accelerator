`ifndef BLK_CTRL_INTERFACE__SV
`define BLK_CTRL_INTERFACE__SV

interface blk_ctrl_interface (input string mode, input string name, input string path);
    logic    ap_done   ;
    logic    ap_idle   ;
    logic    ap_ready  ;
    logic    ap_start  ;
    logic    finish    ;
    logic    ap_continue;
    int      trans_cnt;
    int      ref_trans_cnt;
    integer  file_fp[$]; //this help to close file pointer
    cmd_trace_manager cmd_man_q[$];
    clk_rst_interface clkrst_if(mode, name, path, 1'b1);

    final begin
        if(mode=="tvdump") begin
            integer fp;
            fp = $fopen({path,"/Trace_blk_ctrl_info.dat"}, "w");
            $fdisplay(fp, "transaction_cnt %0d", trans_cnt);
            $fclose(fp);
            //cmd_man doesn't know when simulation is finished
            foreach(cmd_man_q[i]) begin
                if(cmd_man_q[i]) begin
                    cmd_man_q[i].one_trans_result_dumpchk();
                    cmd_man_q[i].finish();
                end
            end

            foreach(file_fp[i])
                if(file_fp[i]) 
                    $fclose(file_fp[i]);
        end
    end
 
    initial begin
        trans_cnt = 0;
        #1ps;
        if(mode.substr(0,5)=="tvread") begin
            integer fp;
            fp = $fopen({path,"/Trace_blk_ctrl_info.dat"}, "r");
            void'($fscanf(fp, "transaction_cnt %d", ref_trans_cnt));
            $fclose(fp);
        end
        @(posedge ap_start);
        trans_cnt++;
        forever begin
            @(posedge clkrst_if.clk);
            #1ps;
            if(ap_start===1 && ap_ready===1) begin
                forever begin
                    @(posedge clkrst_if.clk);
                    #1ps;
                    if(ap_start===1) break;
                end
                trans_cnt++;
            end
        end
    end
endinterface
`endif
