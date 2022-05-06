`ifndef AXI_STATE_SV
`define AXI_STATE_SV

class axi_state_cbs extends uvm_callback;
    function new(string name="axi_state_cbs");
        super.new(name);
    endfunction
    virtual function void memmodel_read_fromar(ref logic[7:0] data[$], input longint addr, input longint len);
    endfunction
endclass

`uvm_analysis_imp_decl(_aw)
`uvm_analysis_imp_decl(_w)
`uvm_analysis_imp_decl(_b)
`uvm_analysis_imp_decl(_ar)
`uvm_analysis_imp_decl(_r)
`uvm_analysis_imp_decl(_aw2b)
`uvm_analysis_imp_decl(_ar2r)

class axi_state extends uvm_component;

    //typedef axi_state_cbs#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH) StateCB;
    //typedef axi_state#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH) AxiStateType;
    //`uvm_register_cb(AxiStateType, StateCB);
    //static local bit m_register_cb_axi_state = uvm_callbacks#(axi_state, axi_state_cbs)::m_register_pair("axi_state","axi_state_cbs");
    `uvm_register_cb(axi_state, axi_state_cbs);

    axi_cfg  cfg;
    //Future improvement: change static array to associate array
    axi_transfer awq        [][$];
    axi_transfer wq         [][$];
    axi_transfer bq_rsp     [][$];
    axi_transfer bq_for_rsp_lat [][$];
    //axi_transfer bq_a2rinfo [][$];

    axi_transfer arq[][$];
    axi_transfer rq [][$];
    axi_transfer rq_from_model[][$];
    int bqnum_for_slaseq;
    event brx_evt;
    int rq_from_model_num;
    int avg_rate;
    int exp_rate;
    int win_size;
    uvm_analysis_imp_aw  #(axi_transfer, axi_state) aw_imp;
    uvm_analysis_imp_w   #(axi_transfer, axi_state) w_imp;
    uvm_analysis_imp_b   #(axi_transfer, axi_state) b_imp;
    uvm_analysis_imp_ar  #(axi_transfer, axi_state) ar_imp;
    uvm_analysis_imp_r   #(axi_transfer, axi_state) r_imp;
    uvm_analysis_imp_aw2b#(axi_transfer, axi_state) aw2b_imp;
    uvm_analysis_imp_ar2r#(axi_transfer, axi_state) ar2r_imp;
    uvm_analysis_port    #(axi_transfer) item_wtr_port;
    uvm_analysis_port    #(axi_transfer) item_rtr_port;

    `uvm_component_utils_begin(axi_state)
        `uvm_field_int(avg_rate , UVM_DEFAULT)
        `uvm_field_int(exp_rate , UVM_DEFAULT)
        `uvm_field_int(win_size , UVM_DEFAULT)
        `uvm_field_int(bqnum_for_slaseq    , UVM_DEFAULT)
        `uvm_field_int(rq_from_model_num, UVM_DEFAULT)
    `uvm_object_utils_end

    function new (string name = "axi_state", uvm_component parent);
        super.new(name, parent);
        aw_imp   = new("aw_imp",this);
        w_imp    = new("w_imp",this);
        b_imp    = new("b_imp",this);
        ar_imp   = new("ar_imp",this);
        r_imp    = new("r_imp",this);
        aw2b_imp = new("aw2b_imp",this);
        ar2r_imp = new("ar2r_imp",this);
        item_wtr_port = new("item_wtr_port", this);
        item_rtr_port = new("item_rtr_port", this);
        bqnum_for_slaseq = 0;
        rq_from_model_num = 0;
    endfunction

    virtual function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        if(!uvm_config_db#(axi_cfg)::get(this, "", "cfg", cfg))
            `uvm_fatal(this.get_full_name(), "axi config must be set for cfg!!!")
        awq           = new[cfg.id_num];
        wq            = new[cfg.id_num];
        bq_rsp        = new[cfg.id_num];
        bq_for_rsp_lat= new[cfg.id_num];
        //bq_a2rinfo    = new[cfg.id_num];
        arq           = new[cfg.id_num];
        rq            = new[cfg.id_num];
        rq_from_model = new[cfg.id_num];
    endfunction

    virtual function void write_aw(axi_transfer tr);
        //if(wq[tr.id].size) begin
        //    if(wq[tr.id].size>1&&wq[tr.id][0].last==0)
        //        `uvm_error("if wq size is bigger than 1, first elem of wq's last should be 1", {wq[tr.id][0].sprint(), wq[tr.id][1].sprint()} )
        //    wq[tr.id][0].unrx_num_cal();
        //    if(wq[tr.id][0].combine_ainfo(tr)) begin
        //        bq_rsp[tr.id].push_back( wq[tr.id].pop_front() );
        //        bqnum_for_slaseq++;
        //        `uvm_info("aw_rx and data is ready", $sformatf("bqnum_for_slaseq:%0d",bqnum_for_slaseq), UVM_MEDIUM)
        //    end
        //end else
        //    awq[tr.id].push_back(tr);

        awq[tr.id].push_back(tr);
        aw_w_merge(tr.id);
    endfunction

    virtual function void aw_w_merge(int id);
        axi_transfer pop_wq_tr; 
        if(awq[id].size && wq[id].size && wq[id][0].ainfo_rx==0) begin
            if(wq[id][0].combine_ainfo(awq[id][0])==3) begin
                pop_wq_tr = wq[id].pop_front();
                bq_rsp[id].push_back( pop_wq_tr );
                bq_for_rsp_lat[id].push_back( pop_wq_tr );
                bqnum_for_slaseq++;
                `uvm_info("aw_w merge and one brsp get", $sformatf("bqnum_for_slaseq:%0d",bqnum_for_slaseq), UVM_MEDIUM)
            end
            void'(awq[id].pop_front());
        end
    endfunction

    virtual function void write_w(axi_transfer tr);
        axi_transfer pop_wq_tr;	
        if(wq[tr.id].size==0|| wq[tr.id][$].last) begin
            wq[tr.id].push_back(tr);
        end else begin
            if(wq[tr.id][$].combine_dinfo(tr)==3) begin
                pop_wq_tr = wq[tr.id].pop_front();
                bq_rsp[tr.id].push_back( pop_wq_tr );
                bq_for_rsp_lat[tr.id].push_back( pop_wq_tr );
                bqnum_for_slaseq++;
                `uvm_info("aw is got already and last data is rx. one brsp get", $sformatf("bqnum_for_slaseq:%0d",bqnum_for_slaseq), UVM_MEDIUM)
                return;
            end
        end
        aw_w_merge(tr.id);
    endfunction

    virtual function void get_bq_ids(output int ids[$]);
        ids.delete();
        for(int i=0; i<cfg.id_num; i++) begin
            if(bq_rsp[i].size) begin
                for(int j=0;j<bq_rsp[i].size; j++) begin
                    if(bq_rsp[i][j].brsp_done==0) begin
                        ids.push_back(i);
                        break;
                    end
                end
            end
        end
    endfunction

    virtual function axi_transfer bq_rsp_pop(int id);
        axi_transfer btr;
        bq_rsp[id][0].brsp_done=1;
        bqnum_for_slaseq--;
        btr = bq_for_rsp_lat[id].pop_front();
        return btr;
    endfunction

    virtual function void write_b(axi_transfer tr);
        if(bq_rsp[tr.id].size==0)
            `uvm_error("bq_rsp can't be empty when b trans is received", "")
        bq_rsp[tr.id][0].combine_binfo(tr);
        //if(bq_rsp[tr.id][0].brsp_done==1)
        //    `uvm_error("first bq_rsp's brsp_done can't be 1", "")
        bq_rsp[tr.id][0].brsp_done = 1;
        //bqnum_for_slaseq--;
        //-> brx_evt;
    endfunction

    virtual function void write_aw2b(axi_transfer tr);
        //bq_a2rinfo[tr.id][0].combine_a2rinfo(tr);
        //item_wtr_port.write(bq_a2rinfo[tr.id].pop_front());
        bq_rsp[tr.id][0].combine_a2rinfo(tr);
        item_wtr_port.write(bq_rsp[tr.id].pop_front());
        //wr_outstanding_cal();
    endfunction

    virtual function void write_ar(axi_transfer tr);
        axi_transfer trclone;
        tr.ainfo_rx = 1;
        tr.expected_read_latency = cfg.clatency.get_rd_lat();
        $cast(trclone, tr.clone());
        trclone.data.delete;
        `uvm_do_callbacks(  axi_state, axi_state_cbs, memmodel_read_fromar(trclone.data, trclone.addr, trclone.get_bytenum())  )
        rq_from_model[trclone.id].push_back(trclone);
        rq_from_model_num++;
        arq[tr.id].push_back(tr);
        `uvm_info("get_one_ar", $sformatf("arq size:%0d", arq[tr.id].size), UVM_MEDIUM)
    endfunction

    virtual function void get_arq_ids(output int ids[$]);
        ids.delete();
        for(int i=0; i<cfg.id_num; i++)
            if(rq_from_model[i].size) ids.push_back(i);
    endfunction

    virtual function axi_transfer get_onedata_from_rqmodel(int id);
        axi_transfer tr;
        int mm;
        if(rq_from_model[id].size) begin
            $cast(tr, rq_from_model[id][0].clone());
            if(tr.data.size==tr.get_bytenum()) mm = tr.get_firstbus_bytenum();
            else mm = 2**tr.size;
            if(tr.data.size<=2**tr.size) begin//only one trans and addr is not alligned with 2**size or last trans
                tr.last = 1;
                void'(rq_from_model[id].pop_front());
                rq_from_model_num--;
            end else begin
                tr.last = 0;
                rq_from_model[id][0].data = rq_from_model[id][0].data[mm:$];
                rq_from_model[id][0].data_delay = rq_from_model[id][0].data_delay[mm:$];
                rq_from_model[id][0].rresp = rq_from_model[id][0].rresp[mm:$];
            end
            if(tr.data.size != tr.get_bytenum()) begin	// is the first transfer
                tr.read_delay = tr.expected_read_latency;
            end  
            tr.data = tr.data[0:mm-1];
            tr.data_delay = tr.data_delay[0:mm-1];
            tr.rresp = tr.rresp[0:mm-1];
        end else `uvm_error("rdmodel queue reading error", $sformatf("no trans is available for id:%0d", id))
        return tr;
    endfunction

    virtual function void write_r(axi_transfer tr);
        if(rq[tr.id].size>0) begin
            for(int i=0; i<rq[tr.id].size; i++) begin
                if(rq[tr.id][i].last==0) begin
                    rq[tr.id][i].combine_dinfo(tr);
                    `uvm_info("get_one_r after address", $sformatf("arq size:%0d, rq size:%0d", arq[tr.id].size, rq[tr.id].size), UVM_MEDIUM)
                    return;
                end
            end
        end
        if(arq[tr.id].size) begin
            //this is first received data corresponding to addr tr
            for(int i=0; i<arq[tr.id][0].addr%arq[tr.id][0].bus_bytenum; i++) tr.data.pop_front();
            arq[tr.id][0].combine_dinfo(tr);
            rq[tr.id].push_back(arq[tr.id].pop_front());
            `uvm_info("get_one_r start", $sformatf("arq size:%0d, rq size:%0d", arq[tr.id].size, rq[tr.id].size), UVM_MEDIUM)
        end else begin
            `uvm_error("can't go into this routine", $sformatf("arq size:%0d, rq size:%0d", arq[tr.id].size, rq[tr.id].size))
            if(rq[tr.id][$].size==0)
                `uvm_error("rq should not be empty when r trans is monitored", tr.sprint() )
            if(rq[tr.id][$].last)
                `uvm_error("if last of rq trans is set, it must be popout", rq[tr.id][$].sprint() )
            if(rq[tr.id][$].ainfo_rx==0)
                `uvm_error("addr must be received before a read data", {tr.sprint(), rq[tr.id][$].sprint()} )
            rq[tr.id][$].combine_dinfo(tr);
        end
    endfunction

    virtual function void write_ar2r(axi_transfer tr);
        `uvm_info("get_one_ar2r begin", $sformatf("arq size:%0d, rq size:%0d", arq[tr.id].size, rq[tr.id].size), UVM_MEDIUM)
        rq[tr.id][0].combine_a2rinfo(tr);
        item_rtr_port.write(rq[tr.id].pop_front());
        `uvm_info("get_one_ar2r", $sformatf("arq size:%0d, rq size:%0d", arq[tr.id].size, rq[tr.id].size), UVM_MEDIUM)
        //rd_oustanding_cal();
    endfunction

    virtual function int wr_outstanding_cal();
        int t = 0;
        for(int i=0; i<cfg.id_num; i++) begin
            t += awq[i].size;
            if(wq[i].size) begin
                for(int j=0; j<wq[i].size; j++) begin
                    if(wq[i][j].ainfo_rx) t++;
                end
            end
            t += bq_rsp[i].size;
            //t += bq_a2rinfo[i].size;
        end
        return t;
    endfunction

    virtual function int rd_outstanding_cal();
        int t = 0;
        for(int i=0; i<cfg.id_num; i++) begin
            t += arq[i].size;
            if(rq[i].size) begin
                for(int j=0; j<rq[i].size; j++) begin
                    if(rq[i][j].ainfo_rx) t++;
                end
            end
        end
        return t;
    endfunction
endclass
`endif
