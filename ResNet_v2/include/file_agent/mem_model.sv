//==============================================================
//Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2.0 (64-bit)
//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//==============================================================
`ifndef MEM_MODEL__SV
`define MEM_MODEL__SV
    
class mem_model#(int WIDTH = 8) extends uvm_object;
    
    logic[WIDTH-1:0] mem[];
    bit IsWr[];
    int depth;
    int offset;
    bit is_rd;

    
    `uvm_object_param_utils_begin(mem_model#(WIDTH))
    `uvm_object_utils_end
    
    function new (string name = "mem_model");
        super.new(name);
    endfunction

    function void init(int depth, int offset=0);
        mem = new[depth];
        IsWr = new[depth];
        for(int i=0; i<depth; i++) begin
            IsWr[i] = 0;
        end
        is_rd = 0;
        this.depth = depth;
        this.offset = offset;
    endfunction

    function bit read_one_elem(ref logic[WIDTH-1:0] data, input longint addrin);
        longint addr;
        addr = addrin - this.offset;
        if(addr<0||addr>=depth) begin
            data = 'hx;
            return 1;
        end
        if(IsWr[addr]==0) begin
            `uvm_warning(this.get_full_name, $sformatf("reading an unwritten address: addr:'d%0d, offset:'d%0d, depth: 'd%0d", 
                                                                   addrin, this.offset, this.depth))
        end
        is_rd = 1;
        data=mem[addr];
        return 0;
    endfunction

    function bit write_one_elem(logic[WIDTH-1:0] data, longint addr);
        addr = addr - this.offset;
        if(addr<0||addr>=depth) return 1;
        mem[addr]=data;
        IsWr[addr]=1;
        return 0;
    endfunction

    //return means the number of un-successful read
    function int read_elems(ref logic[WIDTH-1:0] data[$], input longint addr, input longint length);
        logic[WIDTH-1:0] tmp;
        read_elems = 0;
        data.delete();
        for(int i=0; i<length; i++) begin
            read_elems += read_one_elem(tmp, addr+i);
            data.push_back(tmp);
        end
        if(read_elems>0) `uvm_warning(this.get_full_name, 
                           $sformatf("part of reading is out of boundary: addr:'d%0d, length:'d%0d,offset:'d%0d,depth:'d%0d",
                                                                        addr, length, this.offset, this.depth) )
    endfunction

    function int write_elems(logic[WIDTH-1:0] data[$], longint addr);
        write_elems = 0;
        for(int i=0; i<data.size; i++) begin
            write_elems += write_one_elem(data[i], addr+i);
        end
        if(write_elems>0) `uvm_warning(this.get_full_name, 
                           $sformatf("part of write is out of boundary: addr:'d%0d, length:'d%0d, offset:'d%0d, depth:'d%0d",
                                                                        addr, data.size, this.offset, this.depth) )
    endfunction
endclass

class mem_model_pages#(int DATA_WIDTH=32, int ELE_WIDTH = 8) extends uvm_object;
    mem_model#(ELE_WIDTH) pages[$];
    int rd_page_idx;
    int wr_page_idx;
    file_write_agent#(DATA_WIDTH) FileWr;

    `uvm_object_param_utils_begin(mem_model_pages#(DATA_WIDTH,ELE_WIDTH))
        `uvm_field_int(rd_page_idx, UVM_DEFAULT)
        `uvm_field_int(wr_page_idx, UVM_DEFAULT)
    `uvm_object_utils_end

    function new (string name = "mem_model");
        super.new(name);
        rd_page_idx=0;
        wr_page_idx=0;
    endfunction

    virtual function tvoutdump_atinit(string fpath );
        integer trans_dp[$];
        FileWr = new("FileWr");
        //set a depth which case can never reach
        //will use receive_ap_done to end one transaction dump
        for(int i=0; i<pages.size; i++) trans_dp.push_back(pages[i].depth+1000);
        FileWr.config_file(fpath, trans_dp);
    endfunction

    virtual function void init_pages(int trans_num, int depth, int offset=0);
        int elenum = (DATA_WIDTH+ELE_WIDTH-1)/ELE_WIDTH;
        //if(depth%elenum) `uvm_warning(this.get_full_name(), 
        //                   $sformatf("depth setting is unreasonable: depth: 'd%0d, elenum:'d%0d", depth,elenum))
        for(int i=0; i<trans_num; i++) begin
            mem_model#(ELE_WIDTH) blk = mem_model#(ELE_WIDTH)::type_id::create($sformatf("mem_blk page %0d",i));
            blk.init(depth, offset);
            pages.push_back(blk);
        end
    endfunction

    virtual function void tvinload_pagechk_atinit(string fpath[$], int depth, bit ispagechk=0, int offset=0);
        //need to figureout how many transaction here
        file_read_agent#(DATA_WIDTH)  fr = new("fr");
        int elenum = (DATA_WIDTH+ELE_WIDTH-1)/ELE_WIDTH;
        integer bitw[$];
        //if(depth%elenum) `uvm_warning(this.get_full_name(), 
        //                   $sformatf("depth setting is unreasonable: depth: 'd%0d, elenum:'d%0d", depth,elenum))
        bitw.push_back(DATA_WIDTH);
        fr.config_file(fpath, bitw);
        fr.read_TVIN_file();
        for(int i=0; i<fr.TV_Queue.size; i++) begin
            mem_model#(ELE_WIDTH) blk = mem_model#(ELE_WIDTH)::type_id::create($sformatf("mem_blk page %0d",i));
            //blk.init(fr.TV_Queue[i].size);
            blk.init(depth, offset);
            pages.push_back(blk);
            if(ispagechk) continue;
            for(int j=0; j<fr.TV_Queue[i].size; j++)
                for(int k=0; k<elenum; k++) 
                    blk.write_one_elem( (fr.TV_Queue[i][j]>>(k*ELE_WIDTH)) & (2**ELE_WIDTH-1), j*elenum+k+offset );
        end
    endfunction

    virtual function void incr_rd_page_idx();
        this.rd_page_idx++;
    endfunction

    virtual function void write_elems_frontpage(logic[ELE_WIDTH-1:0] data[$], longint addr);
        if(pages.size==0) `uvm_warning(this.get_full_name(),"no blk memory assigned")
        else pages[0].write_elems(data, addr);
    endfunction 

    virtual function void read_elems_rdpage(ref logic[ELE_WIDTH-1:0] data[$], input longint addr, input longint length);
        pages[rd_page_idx].read_elems(data, addr, length);
    endfunction 

    virtual function void tvout_dump_frontpage(bit true_dump=1);
        logic[DATA_WIDTH-1:0] data;
        logic[ELE_WIDTH-1:0] tmp;
        int elenum = (DATA_WIDTH+ELE_WIDTH-1)/ELE_WIDTH;
        int i;
        if(true_dump) begin
            for(i=0; i<(pages[0].depth+elenum-1)/elenum; i++) begin
                data = 0;
                for(int j=0; j<elenum; j++) begin
                    pages[0].read_one_elem(tmp, i*elenum+j+pages[0].offset);
                    data += (tmp<<j*ELE_WIDTH);
                    if(i+1==(pages[0].depth+elenum-1)/elenum && pages[0].depth%elenum!=0 
                       && j+1== pages[0].depth%elenum) break;
                end
                FileWr.write_TVOUT_data(data);
            end
            FileWr.receive_ap_done();
        end
        void'(pages.pop_front());
        //if(rd_page_idx>0) rd_page_idx--;
        //if ap_ready/ap_done assert at the same cycle, dump may be called first
        rd_page_idx--;
    endfunction

    function void tobusdata(output logic[63:0] databusbit[$], input int pageidx, input int bus_bitwidth);
        int isbig = (ELE_WIDTH > bus_bitwidth);
        logic[63:0] tmp;
        logic[ELE_WIDTH-1:0] tv_onetrans;
        int elemnum = isbig ? (ELE_WIDTH+bus_bitwidth-1)/bus_bitwidth : (bus_bitwidth+ELE_WIDTH-1)/ELE_WIDTH;
        for(int j=0; j<pages[pageidx].depth; j++) begin
            pages[pageidx].read_one_elem(tv_onetrans, j+pages[pageidx].offset);
            if(isbig) begin
                for(int k=0; k<elemnum; k++) begin
                    tmp = (tv_onetrans>>(k*bus_bitwidth))&(2**bus_bitwidth-1);
                    databusbit.push_back(tmp);
                end
            end else begin
                int mod = j%elemnum;
                if(mod==0) tmp = 0;
                tmp += (tv_onetrans<<(mod*ELE_WIDTH));
                if(mod==elemnum-1||j==pages[pageidx].depth-1) databusbit.push_back(tmp);
            end
        end
    endfunction
endclass

class mem_model_pages_with_diffofst#(int DATA_WIDTH=32, int ELE_WIDTH = 8) extends mem_model_pages#(DATA_WIDTH, ELE_WIDTH);
    int recycle_page_ofst[$];
    int whole_page_size;
    int page_ofst[$];
    string maxi_bundlevar_fpath[string];
    int maxi_bundlevar_offset[string];
    int min_offset;


    `uvm_object_param_utils_begin(mem_model_pages_with_diffofst#(DATA_WIDTH,ELE_WIDTH))
        `uvm_field_queue_int(recycle_page_ofst, UVM_DEFAULT)
        `uvm_field_queue_int(page_ofst, UVM_DEFAULT)
        `uvm_field_int(whole_page_size, UVM_DEFAULT)
        //`uvm_field_aa_int_int(ofst_rdpage, UVM_DEFAULT)
    `uvm_object_utils_end

    function new (string name = "mem_model");
        super.new(name);
    endfunction

    virtual function tvoutdump_atinit(string fpath );
        super.tvoutdump_atinit(fpath);
        page_ofst[rd_page_idx]=0;
        min_offset=0;
    endfunction

    virtual function void init_pages(int trans_num, int depth, int offset=0);
        super.init_pages(trans_num, depth, offset);
        page_ofst[rd_page_idx]=0;
    endfunction

    virtual function void tvinload_pagechk_atinit(string fpath[$], int depth, bit ispagechk=0, int offset=0);
        string varname;
        integer bitw[$];
        string  fpath_varoffset[$];
        super.tvinload_pagechk_atinit(fpath, depth, ispagechk, 0);
        page_ofst[rd_page_idx]=0;
        bitw.push_back(64);
        if(maxi_bundlevar_fpath.first(varname))
            do begin
                file_read_agent#(64)  frtmp = new("fr");
                fpath_varoffset.delete();
                fpath_varoffset.push_back(maxi_bundlevar_fpath[varname]);
                frtmp.config_file(fpath_varoffset, bitw);
                frtmp.read_TVIN_file();
                maxi_bundlevar_offset[varname] = frtmp.TV_Queue[0][0];
                min_offset = (min_offset>frtmp.TV_Queue[0][0]) ? frtmp.TV_Queue[0][0] : min_offset;
            end while(maxi_bundlevar_fpath.next(varname));
    endfunction

    virtual function void incr_rd_page_idx();
        super.incr_rd_page_idx();
        if(recycle_page_ofst.size==0) begin
            int tmp[$];
            tmp = page_ofst.max();
            page_ofst.push_back(tmp[$]+whole_page_size);
        end else begin
            page_ofst.push_back(recycle_page_ofst.pop_front());
        end
    endfunction

    virtual function void tvout_dump_frontpage(bit true_dump=1);
        super.tvout_dump_frontpage(true_dump);
        recycle_page_ofst.push_back(page_ofst.pop_front());
    endfunction

    virtual function void read_elems_pipepage(ref logic[ELE_WIDTH-1:0] data[$], input longint addr, input longint length);
        if(pages.size==0) begin
            `uvm_warning(this.get_full_name(),"no blk memory assigned")
            return;
        end
        for(int i=0; i<page_ofst.size; i++) begin
            if(addr>=page_ofst[i]+min_offset && addr<page_ofst[i]+this.whole_page_size+min_offset) begin
                pages[i].read_elems(data, addr-page_ofst[i]-min_offset, length);
                return;
            end
        end
        `uvm_warning(this.get_full_name(),"no blk memory assigned")
    endfunction

    virtual function void write_elems_pipepage(logic[ELE_WIDTH-1:0] data[$], longint addr);
        if(pages.size==0) begin
            `uvm_warning(this.get_full_name(),"no blk memory assigned")
            return;
        end
        for(int i=0; i<page_ofst.size; i++) begin
            if(addr>=page_ofst[i]+min_offset && addr<page_ofst[i]+this.whole_page_size+min_offset) begin
                pages[i].write_elems(data, addr-page_ofst[i]-min_offset);
                return;
            end
        end
        `uvm_warning(this.get_full_name(),"no blk memory assigned")
    endfunction 

endclass
`endif       
