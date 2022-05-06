`ifndef AXI_MASTER_SEQUENCE_LIB_SV
`define AXI_MASTER_SEQUENCE_LIB_SV

class axi_qu5ch_master_sequence extends axi_base_sequence;
    axi_transfer awq[$];
    axi_transfer  wq[$];
    axi_transfer  bq[$];
    axi_transfer arq[$];
    axi_transfer  rq[$];
    bit rd_busy;
    bit wr_busy;
    process job_wr;
    process job_rd;
    `uvm_object_param_utils_begin (axi_qu5ch_master_sequence)
    `uvm_object_utils_end

    function new (string name = "axi_qu5ch_master_sequence");
        super.new(name);
    endfunction

    virtual function int delay_rnd(tr_type_enum tr_type, int div=1);
        int delay;
        if(isusr_delay==PRAGMA_DELAY) begin
            delay=axi_latency/18;
        end else if(isusr_delay==NO_DELAY) begin
            delay=0;
        end else if(
                    tr_type==AW_TR&&aw_usr_delay.size==0 ||
                    tr_type== W_TR&& w_usr_delay.size==0 ||
                    tr_type== B_TR&& b_usr_delay.size==0 ||
                    tr_type==AR_TR&&ar_usr_delay.size==0 ||
                    tr_type== R_TR&& r_usr_delay.size==0
                   ) begin
            void'(std::randomize(delay) with {
                        delay dist {1:=8, 0:=4, (ii/div):=1, (latency/div):=1, [0:(latency/div)]:/5};
                        delay inside {[0:(latency/div)]};
                                             });
        end else begin
            case(tr_type)
                AW_TR: delay = aw_usr_delay.pop_front();
                 W_TR: delay =  w_usr_delay.pop_front();
                 B_TR: delay =  b_usr_delay.pop_front();
                AR_TR: delay = ar_usr_delay.pop_front();
                 R_TR: delay =  r_usr_delay.pop_front();
            endcase
            if(isusr_delay==LEFT_ROUND_DELAY) begin
                case(tr_type)
                    AW_TR: aw_usr_delay.push_back(delay);
                     W_TR:  w_usr_delay.push_back(delay);
                     B_TR:  b_usr_delay.push_back(delay);
                    AR_TR: ar_usr_delay.push_back(delay);
                     R_TR:  r_usr_delay.push_back(delay);
                endcase
            end
        end
        return delay;
    endfunction

    virtual task body();
        axi_info info;
        uvm_object data;
        super.body();
        fork
            begin
                job_wr = process::self();
                fork
                    forever begin
                        axi_transfer axi_awtr;
                        wait(awq.size&&bq.size);
                        axi_awtr = awq.pop_front();
                        axi_awtr.set_item_context(this, p_sequencer.master_awsqr);
                        // axi_awtr.addr_delay = delay_rnd(AW_TR,1);
                        axi_awtr.addr_delay = cfg.clatency.get_wctrl_lat();
                        `uvm_send(axi_awtr);
                        for(int i=0; i<bq.size; i++)
                            if(bq[i].addr_done==0) begin
                                bq[i].addr_done=1;
                                break;
                            end
                    end
                    forever begin
                        axi_transfer axi_wtr;
                        wait(wq.size&&bq.size);
                        axi_wtr = wq.pop_front();
                        axi_wtr.set_item_context(this, p_sequencer.master_wsqr);
                        // axi_wtr.data_delay[0] = delay_rnd(W_TR, axi_wtr.len+1);
                        axi_wtr.data_delay[0] = cfg.clatency.get_wdata_lat();
                        `uvm_send(axi_wtr);
                        if(axi_wtr.last)  begin
                            for(int i=0; i<bq.size; i++)
                                if(bq[i].data_done==0) begin
                                    bq[i].data_done=1;
                                    break;
                                end
                        end
                    end
                    forever begin
                        axi_transfer axi_btr;
                        wr_busy = 0;
                        wait(bq.size);
                        wait(bq[0].addr_done&&bq[0].data_done);
                        wr_busy = 1;
                        axi_btr = bq.pop_front();
                        axi_btr.set_item_context(this, p_sequencer.master_bsqr);
                        // axi_btr.brsp_delay = delay_rnd(B_TR,1);
                        axi_btr.brsp_delay = cfg.clatency.get_wbrsp_lat();
                        `uvm_send(axi_btr);
                    end
                join
            end
            begin
                job_rd = process::self();
                fork
                    forever begin
                        axi_transfer axi_artr;
                        wait(arq.size&&rq.size);
                        axi_artr = arq.pop_front();
                        axi_artr.set_item_context(this, p_sequencer.master_arsqr);
                        // axi_artr.addr_delay = delay_rnd(AR_TR,1);
                        axi_artr.addr_delay = cfg.clatency.get_rctrl_lat();
                        `uvm_send(axi_artr);
                        for(int i=0; i<rq.size; i++) begin
                            rq[i].addr_done = 1;
                            if(rq[i].last) break;
                        end
                    end
                    forever begin
                        axi_transfer axi_rtr;
                        rd_busy =  0;
                        wait(rq.size);
                        wait(rq[0].addr_done);
                        rd_busy =  1;
                        axi_rtr = rq.pop_front();
                        axi_rtr.set_item_context(this, p_sequencer.master_rsqr);
                        // axi_rtr.data_delay[0] = delay_rnd(R_TR,1);
                        axi_rtr.data_delay[0] = cfg.clatency.get_rdata_lat();
                        `uvm_send(axi_rtr);
                    end
                join
            end
        join_none
    endtask
endclass

class axi_busdatas_master_sequence#(int ADDR_DWIDTH=32, int DATA_DWIDTH=32) extends axi_base_sequence;

    axi_qu5ch_master_sequence qu5ch_seq;
    int wrdata_merge_mode;
    int rddata_merge_mode;
    static semaphore rd_sema = new(1); 
    static semaphore wr_sema = new(1); 

    logic[DATA_DWIDTH+ADDR_DWIDTH-1:0] wr_addr_data[$];
    logic[ADDR_DWIDTH-1 :0] rd_addr     [$];

    `uvm_object_param_utils_begin (axi_busdatas_master_sequence#(ADDR_DWIDTH, DATA_DWIDTH))
    `uvm_object_utils_end

    function new (string name = "axi_master_sequence");
        super.new(name);
        wrdata_merge_mode = 0;
        rddata_merge_mode = 0;
    endfunction

    virtual function void datamerge_inavg(logic[DATA_DWIDTH-1:0] tvdata[$], int addrsize, int offset, bit iswr);
        int pre_merged_size = iswr ? wr_addr_data.size : rd_addr.size;
        int cur_merged_idx = 0;
        int pre_merged_idx = 0;
        int addtv_size  = iswr? tvdata.size : addrsize;
        shortreal exp_rate, merged_rate;
        exp_rate = shortreal'(addtv_size)/shortreal'(pre_merged_size+addtv_size);
        for(int j=0; j<addtv_size; j++) begin
            //+1 means we assume j is inserted into merged queue
            //if after assumed insertion, rate is too high, more old data should
            //be inserted
            merged_rate = shortreal'(j+1)/shortreal'(cur_merged_idx+1);
            while(pre_merged_idx<pre_merged_size && merged_rate>exp_rate)  begin
                cur_merged_idx++;
                pre_merged_idx++; //insert an old one
                merged_rate = shortreal'(j+1)/shortreal'(cur_merged_idx+1);
            end
            if(iswr) wr_addr_data.insert(cur_merged_idx++, ((offset+j*(DATA_DWIDTH/8))<<DATA_DWIDTH)+tvdata[j]);
            else     rd_addr.insert(cur_merged_idx++, offset+j*(DATA_DWIDTH/8));
        end
    endfunction

    virtual task wrdata_send();
        wr_sema.get();
        while(wr_addr_data.size) begin
            axi_transfer tr;
            wait(qu5ch_seq.awq.size<3||qu5ch_seq.wq.size<3);
            tr = axi_transfer::type_id::create("aw");
            tr.tr_type = AW_TR;
            tr.addr = wr_addr_data[0][DATA_DWIDTH-1+ADDR_DWIDTH:DATA_DWIDTH];
            tr.rw = WR;
            if(DATA_DWIDTH==8) tr.size = 0;
            else if(DATA_DWIDTH==16) tr.size = 1;
            else if(DATA_DWIDTH==32) tr.size = 2;
            else if(DATA_DWIDTH==64) tr.size = 3;
            else if(DATA_DWIDTH==128) tr.size = 4;
            else if(DATA_DWIDTH==256) tr.size = 5;
            else if(DATA_DWIDTH==512) tr.size = 6;
            else if(DATA_DWIDTH==1024) tr.size = 7;
            else `uvm_fatal("un-supported bus width", "")
            tr.bus_bytenum = DATA_DWIDTH/8;
            qu5ch_seq.awq.push_back(tr);

            tr = axi_transfer::type_id::create("w");
            tr.tr_type = W_TR;
            tr.rw = WR;
            tr.addr = wr_addr_data[0][DATA_DWIDTH-1+ADDR_DWIDTH:DATA_DWIDTH];
            for(int i=0; i<DATA_DWIDTH/8; i++) begin
                tr.data.push_back(wr_addr_data[0][i*8+7 -: 8]);
            end
            tr.strb.push_back(15);
            tr.bus_bytenum = DATA_DWIDTH/8;
            if(DATA_DWIDTH==8) tr.size = 0;
            else if(DATA_DWIDTH==16) tr.size = 1;
            else if(DATA_DWIDTH==32) tr.size = 2;
            else if(DATA_DWIDTH==64) tr.size = 3;
            else if(DATA_DWIDTH==128) tr.size = 4;
            else if(DATA_DWIDTH==256) tr.size = 5;
            else if(DATA_DWIDTH==512) tr.size = 6;
            else if(DATA_DWIDTH==1024) tr.size = 7;
            else `uvm_fatal("un-supported bus width", "")
            tr.last = 1;
            qu5ch_seq.wq.push_back(tr);

            tr = axi_transfer::type_id::create("b");
            tr.tr_type = B_TR;
            tr.bus_bytenum = DATA_DWIDTH/8;
            tr.rw = WR;
            qu5ch_seq.bq.push_back(tr);
            void'(wr_addr_data.pop_front());
        end
        wait(qu5ch_seq.bq.size==0&&qu5ch_seq.wr_busy==0);
        if(qu5ch_seq.job_wr != null) qu5ch_seq.job_wr.kill();
        wr_sema.put();
    endtask

    virtual task rddata_send();
        rd_sema.get();
        while(rd_addr.size) begin
            axi_transfer tr;
            wait(qu5ch_seq.arq.size<3);
            tr = axi_transfer::type_id::create("ar");
            tr.tr_type = AR_TR;
            tr.rw = RD;
            tr.bus_bytenum = DATA_DWIDTH/8;
            tr.addr = rd_addr[0];
            tr.len = 0;
            if(DATA_DWIDTH==8) tr.size = 0;
            else if(DATA_DWIDTH==16) tr.size = 1;
            else if(DATA_DWIDTH==32) tr.size = 2;
            else if(DATA_DWIDTH==64) tr.size = 3;
            else if(DATA_DWIDTH==128) tr.size = 4;
            else if(DATA_DWIDTH==256) tr.size = 5;
            else if(DATA_DWIDTH==512) tr.size = 6;
            else if(DATA_DWIDTH==1024) tr.size = 7;
            else `uvm_fatal("un-supported bus width", "")
            qu5ch_seq.arq.push_back(tr);

            tr = axi_transfer::type_id::create("w");
            tr.tr_type = W_TR;
            tr.addr = rd_addr[0];
            if(DATA_DWIDTH==8) tr.size = 0;
            else if(DATA_DWIDTH==16) tr.size = 1;
            else if(DATA_DWIDTH==32) tr.size = 2;
            else if(DATA_DWIDTH==64) tr.size = 3;
            else if(DATA_DWIDTH==128) tr.size = 4;
            else if(DATA_DWIDTH==256) tr.size = 5;
            else if(DATA_DWIDTH==512) tr.size = 6;
            else if(DATA_DWIDTH==1024) tr.size = 7;
            else `uvm_fatal("un-supported bus width", "")
            tr.last = 1;
            qu5ch_seq.rq.push_back(tr);
            void'(rd_addr.pop_front());
        end
        wait(qu5ch_seq.rq.size==0&&qu5ch_seq.rd_busy==0);
        if(qu5ch_seq.job_rd != null) qu5ch_seq.job_rd.kill();
        rd_sema.put();
    endtask

    virtual task body();
        axi_info info;
        uvm_object data;
        super.body();
        `uvm_create(qu5ch_seq);
        qu5ch_seq.ap_done       = this.ap_done  ;
        qu5ch_seq.ap_ready      = this.ap_ready ;
        qu5ch_seq.finish        = this.finish   ;
        qu5ch_seq.ii                = this.ii               ;
        qu5ch_seq.latency           = this.latency          ;
        qu5ch_seq.axi_latency       = this.axi_latency      ;
        qu5ch_seq.misc_if           = this.misc_if          ;
        qu5ch_seq.aw_usr_delay      = this.aw_usr_delay     ;
        qu5ch_seq. w_usr_delay      = this. w_usr_delay     ;
        qu5ch_seq. b_usr_delay      = this. b_usr_delay     ;
        qu5ch_seq.ar_usr_delay      = this.ar_usr_delay     ;
        qu5ch_seq. r_usr_delay      = this. r_usr_delay     ;
        qu5ch_seq.isusr_delay       = this.isusr_delay      ;
        `uvm_send(qu5ch_seq);
        fork
            wrdata_send();
            rddata_send();
        join
    endtask

endclass

`endif
