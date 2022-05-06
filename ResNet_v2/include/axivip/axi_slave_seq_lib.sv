`ifndef AXI_SLAVE_SEQUENCE_LIB_SV
`define AXI_SLAVE_SEQUENCE_LIB_SV

class axi_slave_sequence#(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1) extends axi_base_sequence;
    virtual interface axi_if#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH) vif;

    `uvm_object_param_utils_begin(axi_slave_sequence#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
    `uvm_object_utils_end

    function new (string name = "axi_slave_sequence");
        super.new(name);
    endfunction

    virtual function int delay_rnd(tr_type_enum tr_type, int div=1);
        int delay;
        if(isusr_delay==PRAGMA_DELAY) begin
            delay=axi_latency/18;
            //case(tr_type)
            //    AW_TR, AR_TR:  delay=axi_latency/18;
            //    W_TR
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
                        delay dist {1:=4, 0:=4, (ii/div):=1, (latency/div):=1, [0:(latency/div)]:/5};
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

    virtual function int wdiv_cal();
        int div;
        for(int i=0; i<cfg.id_num; i++)
            if(state.wq[i].size && state.wq[i][0].ainfo_rx==0) return 1;

        for(int i=0; i<cfg.id_num; i++)
            if(state.awq[i].size) return state.awq[i][0].len + 1;

        for(int i=0; i<cfg.id_num; i++)
            if(state.wq[i].size && state.wq[i][0].ainfo_rx==1) return state.wq[i][0].len+1;
        return 1;
    endfunction
    virtual function int get_bresp_residue_delay_forTrans( axi_transfer tr );
        int delay;
        if(tr.expected_write_latency < tr.write_delay)  delay = 0;
        else                                            delay = tr.expected_write_latency - tr.write_delay;
        return delay;
    endfunction
    
    virtual function int get_rdata_residue_delay_forTrans( axi_transfer tr );
        int delay;
        if(tr.expected_read_latency < tr.read_delay)    delay = 0;
        else                                            delay = tr.expected_read_latency - tr.read_delay;
        return delay;
    endfunction

    virtual task body();
        axi_info info;
        uvm_object data;

        `uvm_info(this.get_full_name(), "body is called", UVM_MEDIUM)
        if(!uvm_config_db#(virtual interface axi_if#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))::get(p_sequencer, "", "vif", vif))
            `uvm_fatal(this.get_full_name(), "axi vif must be set for vif!!!")
        super.body();
        fork
            if (cfg.write_latency_mode == TRANSACTION_FIRST) begin
                forever begin
                    @(posedge vif.clk)
                    for(int i=0; i<cfg.id_num; i++) begin
                        if(state.awq[i].size) begin
                            for(int j=0; j<state.awq[i].size; j++) begin
                                state.awq[i][j].write_delay ++;
                            end
                        end
                    end
                    //	foreach(state.wq);
                    for(int i=0; i<cfg.id_num; i++) begin
                        if(state.wq[i].size) begin
                            for(int j=0; j<state.wq[i].size; j++) begin
                                if(state.wq[i][j].ainfo_rx==1)  begin
                                    state.wq[i][j].write_delay ++;
                                end
                            end
                        end
                    end
                    //	foreach(state.bq_for_rsp_lat);
                    for(int i=0; i<cfg.id_num; i++) begin
                        if(state.bq_for_rsp_lat[i].size) begin
                            for(int j=0; j<state.bq_for_rsp_lat[i].size; j++) begin
                                state.bq_for_rsp_lat[i][j].write_delay ++;
                            end
                        end
                    end
                end
            end else begin
                @finish;
            end
            if (cfg.read_latency_mode == TRANSACTION_FIRST) begin
                forever begin
                    @(posedge vif.clk)
                    //	foreach(state.rq_from_model)
                    for(int i=0; i<cfg.id_num; i++) begin
                        if(state.rq_from_model[i].size) begin
                            for(int j=0; j<state.rq_from_model[i].size; j++) begin
                                state.rq_from_model[i][j].read_delay ++;
                            end
                        end
                    end
                end
            end else begin
                @finish;
            end
            forever begin
                axi_transfer axi_awtr;
                `uvm_create_on(axi_awtr, p_sequencer.slave_awsqr);
                axi_awtr.tr_type = AW_TR;
                //axi_awtr.addr_delay = delay_rnd(AW_TR, 1);
                axi_awtr.addr_delay = cfg.clatency.get_wctrl_lat();
                `uvm_info(this.get_full_name(), {"send trans:", axi_awtr.sprint}, UVM_MEDIUM)
                `uvm_send(axi_awtr);
            end
            forever begin
                int div;
                axi_transfer axi_wtr;
                `uvm_create_on(axi_wtr, p_sequencer.slave_wsqr);
                axi_wtr.tr_type = W_TR;
                div = wdiv_cal();
                //  axi_wtr.data_delay[0] = delay_rnd(W_TR,div);
                axi_wtr.data_delay[0] = cfg.clatency.get_wdata_lat();
                `uvm_info(this.get_full_name(), {"send trans:", axi_wtr.sprint}, UVM_MEDIUM)
                `uvm_send(axi_wtr);
            end
            forever begin
                int ids[$];
                int div;
                int idx;
                axi_transfer axi_btr;
                axi_transfer tmp_btr;
                `uvm_info(this.get_full_name(), "bqnum_for_slaseq wait begin", UVM_MEDIUM)
                wait(state.bqnum_for_slaseq>0);
                `uvm_create_on(axi_btr, p_sequencer.slave_bsqr);
                state.get_bq_ids(ids);
                axi_btr.tr_type = B_TR;
                void'(std::randomize(idx) with { idx inside {[0:ids.size-1]}; });
                axi_btr.id = ids[idx];
                tmp_btr = state.bq_rsp_pop(ids[idx]);
                //  axi_btr.brsp_delay = delay_rnd(B_TR,1);
                if (cfg.write_latency_mode == TRANSACTION_FIRST) begin //  if latency was set.
                    axi_btr.expected_write_latency = cfg.clatency.get_wr_lat();
                    axi_btr.write_delay = tmp_btr.write_delay; 
                    axi_btr.brsp_delay = get_bresp_residue_delay_forTrans(axi_btr);
                end else begin
                    axi_btr.brsp_delay = cfg.clatency.get_wbrsp_lat();
                end  
                `uvm_info(this.get_full_name(), {"send trans:", axi_btr.sprint}, UVM_MEDIUM)
                `uvm_send(axi_btr);
            end
            forever begin
                int div;
                axi_transfer axi_artr;
                `uvm_create_on(axi_artr, p_sequencer.slave_arsqr);
                axi_artr.tr_type = AR_TR;
                //  axi_artr.addr_delay = delay_rnd(AR_TR,1);
                axi_artr.addr_delay = cfg.clatency.get_rctrl_lat();
                `uvm_info(this.get_full_name(), {"send trans:", axi_artr.sprint}, UVM_MEDIUM)
                `uvm_send(axi_artr);
            end
            forever begin
                int ids[$];
                int idx;
                int div;
                axi_transfer axi_rtr, tmp;
                `uvm_create_on(axi_rtr, p_sequencer.slave_rsqr);
                wait(state.rq_from_model_num>0);
                state.get_arq_ids(ids);
                axi_rtr.tr_type = R_TR;
                void'(std::randomize(idx) with { idx inside {[0:ids.size-1]}; });
                tmp = state.get_onedata_from_rqmodel(ids[idx]);
                axi_rtr.combine_dinfo(tmp);
                axi_rtr.id = ids[idx];
                //  axi_rtr.data_delay[0] = delay_rnd(R_TR, tmp.len+1);
                if (cfg.read_latency_mode == TRANSACTION_FIRST) begin //  if latency was set!!!!!
                    axi_rtr.data_delay[0] = get_rdata_residue_delay_forTrans( tmp );
                end else begin
                    axi_rtr.data_delay[0] = cfg.clatency.get_rdata_lat();
                end
                `uvm_info(this.get_full_name(), {"send trans:", axi_rtr.sprint}, UVM_MEDIUM)
                `uvm_send(axi_rtr);
            end
            begin
                @finish;
            end
        join_any
        disable fork;
        //p_sequencer..stop_sequences();
        //p_sequencer..stop_sequences();
        //disable fork;
        //disable fork;
    endtask

endclass

`endif
