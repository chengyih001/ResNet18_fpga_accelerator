`ifndef AXI_TRANSFER_SV
`define AXI_TRANSFER_SV

class axi_transfer extends uvm_sequence_item;
    rand bit                             addr_done;
    rand bit                             data_done;
    rand bit                             brsp_done;
    rand tr_type_enum                    tr_type;
    rand bit                             ainfo_rx;
    rand int                             unrx_num;
    rand direction_enum                  rw;
    rand int                             id;
    rand longint                         addr;
    rand int                             len;
    rand int                             size;
    rand burst_enum                      burst;
    rand lock_enum                       lock;
    rand int                             region;
    rand cache_enc_enum                  cache;
    rand int                             prot;
    rand int                             qos;
    rand resp_enum                       bresp;
    rand resp_enum                       rresp[$];
    rand int                             strb[$];
    rand logic [7:0]                     data[$];
    rand int                             data_delay[$];
    rand int                             addr_delay;
    rand int                             brsp_delay;
    rand int                             a2r_delay; //addr to response
    rand bit                             last;
    rand int                             bus_bytenum;
    rand int                             user;
    rand int                             write_delay;
    rand int                             read_delay;
    rand int                             expected_write_latency;
    rand int                             expected_read_latency;
    
    `uvm_object_utils_begin(axi_transfer)
      `uvm_field_int  (addr_done,           UVM_DEFAULT)
      `uvm_field_int  (data_done,           UVM_DEFAULT)
      `uvm_field_int  (brsp_done,           UVM_DEFAULT)
      `uvm_field_int  (ainfo_rx,            UVM_DEFAULT)
      `uvm_field_enum (tr_type_enum,tr_type,UVM_DEFAULT)
      `uvm_field_int  (ainfo_rx,            UVM_DEFAULT)
      `uvm_field_int  (unrx_num,            UVM_DEFAULT)
      `uvm_field_enum (direction_enum, rw,  UVM_DEFAULT)
      `uvm_field_int  (id,                  UVM_DEFAULT)
      `uvm_field_int  (addr,                UVM_DEFAULT)
      `uvm_field_int  (len,                 UVM_DEFAULT)
      `uvm_field_int  (size,                UVM_DEFAULT)
      `uvm_field_enum (burst_enum, burst,   UVM_DEFAULT)
      `uvm_field_enum (lock_enum, lock,     UVM_DEFAULT)
      `uvm_field_int  (region,              UVM_DEFAULT)
      `uvm_field_enum (cache_enc_enum, cache, UVM_DEFAULT)
      `uvm_field_int  (prot,                UVM_DEFAULT)
      `uvm_field_int  (qos,                 UVM_DEFAULT)
      `uvm_field_enum (resp_enum,bresp,     UVM_DEFAULT)
      `uvm_field_queue_enum (resp_enum,rresp, UVM_DEFAULT)
      `uvm_field_queue_int(strb,            UVM_DEFAULT)
      `uvm_field_queue_int(data,            UVM_DEFAULT)
      `uvm_field_queue_int(data_delay,      UVM_DEFAULT)
      `uvm_field_int  (addr_delay,          UVM_DEFAULT)
      `uvm_field_int  (brsp_delay,          UVM_DEFAULT)
      `uvm_field_int  (a2r_delay,           UVM_DEFAULT)
      `uvm_field_int  (bus_bytenum,         UVM_DEFAULT)
      `uvm_field_int  (last,                UVM_DEFAULT)
      `uvm_field_int  (user,                UVM_DEFAULT)
      `uvm_field_int  (write_delay,         UVM_DEFAULT)
      `uvm_field_int  (read_delay,         UVM_DEFAULT)
      `uvm_field_int  (expected_write_latency,      UVM_DEFAULT)
      `uvm_field_int  (expected_read_latency,       UVM_DEFAULT)
    `uvm_object_utils_end

    function new (string name = "axi_transfer");
      super.new(name);
      init();
    endfunction

    function void init();
        addr_done=0;
        data_done=0;
        brsp_done=0;
        tr_type  = AR_TR;
        ainfo_rx = 0;
        unrx_num = 0;
        rw       = RD;
        id       = 0;
        addr     = 0;
        len      = 0;
        size     = 0;
        burst    = FIXED;
        lock     = NORMAL;
        region   = 0;
        cache    = WA0_RA0_C0_B0;
        prot     = 0;
        qos      = 0;
        bresp    = OKAY;
        rresp.delete;
        strb.delete ;
        data.delete ;
        data_delay.delete;
        addr_delay = 0;
        brsp_delay = 0;
        a2r_delay = 0;
        bus_bytenum = 0;
        last       = 0;
    
        write_delay = 0;
        read_delay = 0;
        expected_write_latency = 0;
        expected_read_latency = 0;
    endfunction

    function int combine_ainfo(axi_transfer tr);
        combine_ainfo = 0;
        this.addr    = tr.addr    ;
        this.region  = tr.region  ;
        this.len     = tr.len     ;
        this.size    = tr.size    ;
        this.burst   = tr.burst   ;
        this.lock    = tr.lock    ;
        this.cache   = tr.cache   ;
        this.prot    = tr.prot    ;
        this.qos     = tr.qos     ;
        this.addr_delay = tr.addr_delay;
        this.ainfo_rx = 1;
        this.write_delay = tr.write_delay;
        this.expected_write_latency = tr.expected_write_latency;
        if(this.rw==WR  && this.data.size!=0) waddr_refine(this.strb[0]);
        if(this.rw==WR) begin
            int unstrob_msb = this.addr%this.bus_bytenum;
            if( (this.strb[0] & ((1<<unstrob_msb)-1)) !=0 )
                `uvm_error("first wdata strobe setting error for unaligned transfer", "")
        end
        if(this.last==1) begin
            if(this.get_bytenum()!=this.data.size&&this.len!=0&&this.burst==INCR) `uvm_error("last bit err", this.sprint())
            if(this.rw == RD) combine_ainfo = 4; //bit0 addr bit 1 data bit 2 all
            else              combine_ainfo = 3;
        end
    endfunction

    function void combine_a2rinfo(axi_transfer tr);
        this.a2r_delay = tr.a2r_delay;
        if(rw==RD) this.tr_type = R_ALL;
        else       this.tr_type = W_ALL;
    endfunction

    function void combine_binfo(axi_transfer tr);
        this.bresp     = tr.bresp;
        this.brsp_delay= tr.brsp_delay;
        if(this.get_bytenum() != this.data.size&&this.len!=0&&this.burst==INCR||this.last!=1)
            `uvm_error("last bit or data size/len unmatch err", this.sprint())
    endfunction

    function int combine_dinfo(axi_transfer tr);
        int bytenum = 2**size;
        combine_dinfo = 0;
        if(this.rw==WR && ainfo_rx && this.data.size==0) waddr_refine(tr.strb[0]);
        this.data = {this.data, tr.data};
        this.data_delay = {this.data_delay,tr.data_delay};
        this.rresp = {this.rresp, tr.rresp};
        this.strb  = {this.strb,  tr.strb };
        this.last = tr.last;
        if(ainfo_rx&&this.last) begin
            if(this.get_bytenum() !=this.data.size) `uvm_error("last bit err", this.sprint())
            if(this.rw == RD) combine_dinfo = 4; //bit0 addr bit 1 data bit 2 all
            else              combine_dinfo = 3;
        end
    endfunction

    function void waddr_refine(int strb);
        int bytenum = 2**size;
        this.addr = (this.addr/this.bus_bytenum)*this.bus_bytenum;
        for(int i=0; i<this.bus_bytenum; i++) begin
            if( ((1<<i)&strb)==0 ) this.addr++;
            else break;
        end
    endfunction

    function int get_bytenum();
        int actlen = this.len+1;
        int bytenum = 2**size;
        return actlen*bytenum - (this.addr%bytenum);
    endfunction

    function int get_firstbus_bytenum();
        int bytenum = 2**this.size;
        return bytenum - (addr%bytenum);
    endfunction

    function unrx_num_cal();
        int bytenum = 2**this.size;
        this.unrx_num = this.len+1 - (this.data.size+bytenum-1)/bytenum;
    endfunction

    // function void bytedata2bus(input logic[7:0] data[$]);
    //     int j = this.addr%STRB_WIDTH;
    //     for(int i=0; i<j; i++) begin
    //         this.data[0][i*8+7-:8]='hx;
    //     end
    //     for(int i=0; i<this.get_bytenum(); i++) begin
    //         this.data[j/STRB_WIDTH][(j%STRB_WIDTH)*8+7-:8]=data.pop_front();
    //         j++;
    //     end
    // endfunction

    // function void bus2bytedata(output logic[7:0] data[$]);
    //     int j = this.addr%STRB_WIDTH;
    //     for(int i=0; i<this.get_bytenum(); i++) begin
    //         data.push_back(this.data[j/STRB_WIDTH][(j%STRB_WIDTH)*8+7-:8]);
    //         j++;
    //     end
    // endfunction

endclass : axi_transfer


`endif


