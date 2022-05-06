`ifndef SVR_MASTER_SEQUENCE__SV
    `define SVR_MASTER_SEQUENCE__SV
    
    class svr_master_sequence#(int DATA_WIDTH = 32) extends svr_base_sequence#(DATA_WIDTH);
        
        svr_transfer#(DATA_WIDTH) svr_trans;
        svr_config cfg;
        DATA_TYPE data;
        int one_sect_read;
        
        `uvm_object_param_utils_begin (svr_master_sequence#(DATA_WIDTH))
        `uvm_object_utils_end
        
        function new (string name = "svr_master_sequence");
            super.new(name);
            one_sect_read = 0;
            `uvm_info(this.get_full_name(), "new is called", UVM_LOW)
        endfunction
        
        virtual task body();
            int delayidx = 0;
            int total_tv_size=0;
            int sent_tv_size=0;
            `uvm_info(this.get_full_name(), "body is called", UVM_LOW)
            if(!uvm_config_db #(svr_config)::get(m_sequencer, "", "cfg", cfg))
                `uvm_fatal(this.get_full_name(), "svr config must be set for cfg!!!") 
                
            read_file();
            total_tv_size = left_data_num();
            fork
                begin
                    @finish;
                    if(total_tv_size>sent_tv_size) 
                        `uvm_error("CRITICAL WARNNING", "Array input is set to stream interface and array data is not all used. If simulation fails, please check array size")
                end
            join_none

            while (get_TestVector_num() > 0) begin
                pop_TestVector();
                `uvm_info(this.get_full_name(), $sformatf("switch to new transaction with size %0d", get_TestVector_size()), UVM_MEDIUM)
                fork
                    begin
                        //if(cfg.prt_type==AP_VLD && misc_if.tb2dut_ap_start==0) wait(misc_if.tb2dut_ap_start===1);
                        while (get_TestVector_size() > 0) begin
                            int delay;
                            data = pop_data();
                            `uvm_create(svr_trans);
                            
                            // if(isusr_delay==NO_DELAY) svr_trans.delay = 0;
                            // else if(usr_delay.size==0) begin
                            //     void'(std::randomize(delay) with {
                            //                 delay dist {1:=4, 0:=4, ii:=1, latency:=1, [0:latency]:/5};
                            //                 delay inside {[0:latency]};
                            //                                      });
                            //     svr_trans.delay = delay;
                            // end else begin
                            //     svr_trans.delay = usr_delay.pop_front();
                            //     if(isusr_delay==LEFT_ROUND_DELAY) usr_delay.push_back(svr_trans.delay);
                            // end
                            svr_trans.delay = cfg.clatency.get_transfer_lat();
                            svr_trans.data  = data;
                            `uvm_info(this.get_full_name(), {"send trans:", svr_trans.sprint}, UVM_MEDIUM)
                            `uvm_send(svr_trans);
                            sent_tv_size++;
                            //if(cfg.prt_type==AP_HS||cfg.prt_type==AP_FIFO) begin
                            //    if(svr_evt_ap_ready.is_on()&&get_TestVector_size()>0) begin
                            //        `uvm_error(this.get_full_name(), $sformatf("DUT needs less data than vector provide, left data number:%0d", get_TestVector_size()))
                            //    end
                            //end
                        end
                        one_sect_read = 1;
                    end
                    begin
                        case( cfg.prt_type )
                            AP_VLD, AP_NONE, AP_HS, AP_FIFO, AXIS: begin
                                `uvm_info(this.get_full_name(), $sformatf("wait for ap_ready"), UVM_MEDIUM)
                                @ap_ready;
                                //svr_evt_ap_ready.wait_on();
                                //evtdata = svr_evt_ap_ready.get_trigger_data();
                                //svr_evt_ap_ready.reset();
                            end
                        endcase
                    end
                join
            end
            `uvm_info(this.get_full_name(), "send all TVs done", UVM_LOW)
        endtask
        
    endclass
     
`endif
