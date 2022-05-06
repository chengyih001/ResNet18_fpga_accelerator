`ifndef CLK_RST_INTERFACE__SV
`define CLK_RST_INTERFACE__SV

interface clk_rst_interface (input string mode, input string name, input string path, input bit clkrst_trace);
//timeunit 1ps;
//timeprecision  1ps;
logic clk;
logic rst;
longint clkcnt;
integer fp;
initial begin
    bit clkdone=0;
    #1ps;
    if(mode=="tvdump" && clkrst_trace) begin
        fp = $fopen({path,"/Trace_", name, "_clkrst.dat"}, "w");
        if(!fp) begin
            $display("ERROR: open %s/Trace_%s_clkrst.dat failed", path, name);
            $finish;
        end
        $fdisplay(fp, "rst_init %0d", clk);
        $fdisplay(fp, "clk_init %0d", rst);
        fork
            begin
                time tt1;
                time tt2;
                @(posedge clk);
                tt1 = $realtime;
                @(posedge clk);
                tt2 = $realtime;
                $fdisplay(fp, "clk_cyc %t", tt2-tt1);
                clkdone=1;
            end
            begin
                time t_rst;
                logic rstvalue;
                @(rst);
                t_rst = $realtime;
                rstvalue = rst;
                wait(clkdone);
                $fdisplay(fp,"rst_change %t %0d", t_rst, rstvalue);
            end
        join
        $fclose(fp);
    end
    if(mode.substr(0,5)=="tvread" && clkrst_trace) begin
        logic clkinit;
        logic rstinit;
        //vcs_pass_code time  clkcyc;
        //vcs_pass_code time  rst_deassert;
        //longint  clkcyc=3334;
        //longint  rst_deassert=518537;
        string tmp;
        longint  clkcyc;
        longint  rst_deassert;
        logic rst_deassert_val;
        fp = $fopen({path,"/Trace_", name, "_clkrst.dat"}, "r");
        if(!fp) begin
            $display("ERROR: open %s/Trace_%s_clkrst.dat failed", path, name);
            $finish;
        end
        void'($fscanf(fp, "%s %d", tmp, rstinit));
        void'($fscanf(fp, "%s %d", tmp, clkinit));
        //vcs_pass_code void'($fscanf(fp, "clk_cyc %t", clkcyc));
        //vcs_pass_code void'($fscanf(fp, "rst_change %t %d", rst_deassert, rst_deassert_val));
        //  void'($fscanf(fp, "clk_cyc %d", clkcyc));
        //  void'($fscanf(fp, "rst_change %d %d", rst_deassert, rst_deassert_val));
        void'($fscanf(fp, "%s %d",tmp, clkcyc));
        void'($fscanf(fp, "%s %d %d",tmp, rst_deassert, rst_deassert_val));
        $fclose(fp);
        $display("lineww65: clkcyc:%0d, rst_deassert:%0d, clkinit:%0d, rstinit:%0d", clkcyc, rst_deassert, clkinit, rstinit);
        clk = clkinit;
        rst = rstinit;
        fork
            forever begin
                #((clkcyc/2)*1ps);
                clk = ~clk;
            end
            begin
                //#(rst_deassert-3*clkcyc+2365474ps);
                #((rst_deassert-3*clkcyc)*1ps);
                rst = rst_deassert_val;
            end
        join
    end
end

final begin
    if(fp) $fclose(fp);
end

initial begin
    clkcnt = 0;
    forever begin
        @(posedge clk);
        #1ps;
        clkcnt++;
    end
end
endinterface

`endif
