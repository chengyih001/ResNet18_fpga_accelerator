`ifndef VLD_RDY_DATA_TRACE_PHY_MONDRV__SV
`define VLD_RDY_DATA_TRACE_PHY_MONDRV__SV

`define DRV_DELAY 0ps
`define ERRCNT 4
class vld_rdy_data#(int DATAW=8) extends data_base_object;
    logic[DATAW-1:0] datain;
    bit isseg;
    logic[DATAW-1:0] data_eachf[string];
    string DataField[$];
    int    DataBitWidth[$];

    function new();
        isseg = 0;
    endfunction

    virtual function void update_data(logic[DATAW-1:0] data);
        this.datain = data;
        isseg = 0;
        data_eachf.delete();
    endfunction

    virtual function void data_segment();
        logic[DATAW-1:0] tmp = this.datain;
        foreach(DataField[i]) begin
            data_eachf[DataField[i]] = tmp & ((1<<DataBitWidth[i])-1);
            tmp = tmp>>DataBitWidth[i];
        end
        isseg = 1;
    endfunction

    virtual function logic[DATAW-1:0] get_fielddata(string field);
        if(isseg==0) this.data_segment();
        return data_eachf[field];
    endfunction
endclass

class vld_rdy_data_trace_file_agent #(int DATAW=8);
    string FilePath;
    string mode;

    virtual function void file_cfg(string fpath, string mode);
        this.FilePath = fpath;
        this.mode     = mode;
    endfunction

    //print the head information or get the head line
    virtual function void init_head_proc(string evtf[$], string dataf[$]);
        if(mode=="tvdump") begin
            integer fp;
            fp = $fopen(FilePath, "w"); 
            foreach(evtf[i])
                if(i==0) $fwrite(fp, "//%s\t", evtf[i]);
                else     $fwrite(fp, "%s\t", evtf[i]) ;
    
            foreach(dataf[i])
                if(i==dataf.size-1) $fwrite(fp, "%s\n", dataf[i]);
                else                    $fwrite(fp, "%s\t", dataf[i]);  
            $fclose(fp); ///maybe deleted later
        end
        if(mode=="tvread") begin
            //string line;
            //fp = $fopen(FilePath, "r"); 
            //$fgets(line, fp); //read head line
        end
    endfunction

    virtual function void dump_oneline(longint clkcyc[string][$], logic[DATAW-1:0] data, string evtf[$], int dataw[$]);
        integer fp;
        int shift = 0;
        logic[DATAW-1:0] tmp;

        if(mode=="tvread") return;

//$display("lineww, onelinedump to file:%s at %t. TB file:%s line:%d", FilePath, $time, `__FILE__, `__LINE__ );

        fp = $fopen(FilePath, "a"); 
        if(fp) begin
            foreach(evtf[i]) begin
//$display("lineww, %s size:%0d, first:%0h, :%s at %t. TB file:%s line:%d", 
//               evtf[i], clkcyc[evtf[i]].size, clkcyc[evtf[i]][0],  FilePath, $time, `__FILE__, `__LINE__ );
                $fwrite(fp, "%16h\t", clkcyc[evtf[i]][0] );
            end
            
            foreach(dataw[i]) begin
                tmp = data & ((1<<dataw[i])-1);
                data = data >> dataw[i];
                     if(dataw[i] <=  16) $fwrite(fp, "%4h", tmp);
                else if(dataw[i] <=  32) $fwrite(fp, "%8h", tmp);
                else if(dataw[i] <=  48) $fwrite(fp, "%12h", tmp);
                else if(dataw[i] <=  64) $fwrite(fp, "%16h", tmp);
                else if(dataw[i] <=  80) $fwrite(fp, "%20h", tmp);
                else if(dataw[i] <=  96) $fwrite(fp, "%24h", tmp);
                else if(dataw[i] <= 112) $fwrite(fp, "%28h", tmp);
                else if(dataw[i] <= 128) $fwrite(fp, "%32h", tmp);
                else if(dataw[i] <= 144) $fwrite(fp, "%36h", tmp);
                else if(dataw[i] <= 160) $fwrite(fp, "%40h", tmp);
                else if(dataw[i] <= 176) $fwrite(fp, "%44h", tmp);
                else if(dataw[i] <= 192) $fwrite(fp, "%48h", tmp);
                else if(dataw[i] <= 208) $fwrite(fp, "%52h", tmp);
                else if(dataw[i] <= 224) $fwrite(fp, "%56h", tmp);
                else if(dataw[i] <= 240) $fwrite(fp, "%60h", tmp);
                else if(dataw[i] <= 256) $fwrite(fp, "%64h", tmp);
                else if(dataw[i] <= 272) $fwrite(fp, "%68h", tmp);
                else if(dataw[i] <= 288) $fwrite(fp, "%72h", tmp);
                else if(dataw[i] <= 304) $fwrite(fp, "%76h", tmp);
                else if(dataw[i] <= 320) $fwrite(fp, "%80h", tmp);
                else if(dataw[i] <= 336) $fwrite(fp, "%84h", tmp);
                else if(dataw[i] <= 352) $fwrite(fp, "%88h", tmp);
                else if(dataw[i] <= 368) $fwrite(fp, "%92h", tmp);
                else if(dataw[i] <= 384) $fwrite(fp, "%96h", tmp);
                else if(dataw[i] <= 400) $fwrite(fp, "%100h", tmp);
                else if(dataw[i] <= 416) $fwrite(fp, "%104h", tmp);
                else if(dataw[i] <= 432) $fwrite(fp, "%108h", tmp);
                else if(dataw[i] <= 448) $fwrite(fp, "%112h", tmp);
                else if(dataw[i] <= 464) $fwrite(fp, "%116h", tmp);
                else if(dataw[i] <= 480) $fwrite(fp, "%120h", tmp);
                else if(dataw[i] <= 496) $fwrite(fp, "%124h", tmp);
                else if(dataw[i] <= 512) $fwrite(fp, "%128h", tmp);
                else if(dataw[i] <= 528) $fwrite(fp, "%132h", tmp);
                else if(dataw[i] <= 544) $fwrite(fp, "%136h", tmp);
                else if(dataw[i] <= 560) $fwrite(fp, "%140h", tmp);
                else if(dataw[i] <= 576) $fwrite(fp, "%144h", tmp);
                else if(dataw[i] <= 592) $fwrite(fp, "%148h", tmp);
                else if(dataw[i] <= 608) $fwrite(fp, "%152h", tmp);
                else if(dataw[i] <= 624) $fwrite(fp, "%156h", tmp);
                else if(dataw[i] <= 640) $fwrite(fp, "%160h", tmp);
                else if(dataw[i] <= 656) $fwrite(fp, "%164h", tmp);
                else if(dataw[i] <= 672) $fwrite(fp, "%168h", tmp);
                else if(dataw[i] <= 688) $fwrite(fp, "%172h", tmp);
                else if(dataw[i] <= 704) $fwrite(fp, "%176h", tmp);
                else if(dataw[i] <= 720) $fwrite(fp, "%180h", tmp);
                else if(dataw[i] <= 736) $fwrite(fp, "%184h", tmp);
                else if(dataw[i] <= 752) $fwrite(fp, "%188h", tmp);
                else if(dataw[i] <= 768) $fwrite(fp, "%192h", tmp);
                else if(dataw[i] <= 784) $fwrite(fp, "%196h", tmp);
                else if(dataw[i] <= 800) $fwrite(fp, "%200h", tmp);
                else if(dataw[i] <= 816) $fwrite(fp, "%204h", tmp);
                else if(dataw[i] <= 832) $fwrite(fp, "%208h", tmp);
                else if(dataw[i] <= 848) $fwrite(fp, "%212h", tmp);
                else if(dataw[i] <= 864) $fwrite(fp, "%216h", tmp);
                else if(dataw[i] <= 880) $fwrite(fp, "%220h", tmp);
                else if(dataw[i] <= 896) $fwrite(fp, "%224h", tmp);
                else if(dataw[i] <= 912) $fwrite(fp, "%228h", tmp);
                else if(dataw[i] <= 928) $fwrite(fp, "%232h", tmp);
                else if(dataw[i] <= 944) $fwrite(fp, "%236h", tmp);
                else if(dataw[i] <= 960) $fwrite(fp, "%240h", tmp);
                else if(dataw[i] <= 976) $fwrite(fp, "%244h", tmp);
                else if(dataw[i] <= 992) $fwrite(fp, "%248h", tmp);
                else if(dataw[i] <=1008) $fwrite(fp, "%252h", tmp);
                else if(dataw[i] <=1024) $fwrite(fp, "%256h", tmp);
                else if(dataw[i] <=1040) $fwrite(fp, "%260h", tmp);
                else if(dataw[i] <=1056) $fwrite(fp, "%264h", tmp);
                else if(dataw[i] <=1072) $fwrite(fp, "%268h", tmp);
                else if(dataw[i] <=1088) $fwrite(fp, "%272h", tmp);
                else if(dataw[i] <=1104) $fwrite(fp, "%276h", tmp);
                else if(dataw[i] <=1120) $fwrite(fp, "%280h", tmp);
                else if(dataw[i] <=1136) $fwrite(fp, "%284h", tmp);
                else if(dataw[i] <=1152) $fwrite(fp, "%288h", tmp);
                else if(dataw[i] <=1168) $fwrite(fp, "%292h", tmp);
                else if(dataw[i] <=1184) $fwrite(fp, "%296h", tmp);
                else if(dataw[i] <=1200) $fwrite(fp, "%300h", tmp);
                else if(dataw[i] <=1216) $fwrite(fp, "%304h", tmp);
                else if(dataw[i] <=1232) $fwrite(fp, "%308h", tmp);
                else if(dataw[i] <=1248) $fwrite(fp, "%312h", tmp);
                else if(dataw[i] <=1264) $fwrite(fp, "%316h", tmp);
                else if(dataw[i] <=1280) $fwrite(fp, "%320h", tmp);
                else if(dataw[i] <=1296) $fwrite(fp, "%324h", tmp);
                else if(dataw[i] <=1312) $fwrite(fp, "%328h", tmp);
                else if(dataw[i] <=1328) $fwrite(fp, "%332h", tmp);
                else if(dataw[i] <=1344) $fwrite(fp, "%336h", tmp);
                else if(dataw[i] <=1360) $fwrite(fp, "%340h", tmp);
                else if(dataw[i] <=1376) $fwrite(fp, "%344h", tmp);
                else if(dataw[i] <=1392) $fwrite(fp, "%348h", tmp);
                else if(dataw[i] <=1408) $fwrite(fp, "%352h", tmp);
                else if(dataw[i] <=1424) $fwrite(fp, "%356h", tmp);
                else if(dataw[i] <=1440) $fwrite(fp, "%360h", tmp);
                else if(dataw[i] <=1456) $fwrite(fp, "%364h", tmp);
                else if(dataw[i] <=1472) $fwrite(fp, "%368h", tmp);
                else if(dataw[i] <=1488) $fwrite(fp, "%372h", tmp);
                else if(dataw[i] <=1504) $fwrite(fp, "%376h", tmp);
                else if(dataw[i] <=1520) $fwrite(fp, "%380h", tmp);
                else if(dataw[i] <=1536) $fwrite(fp, "%384h", tmp);
                else if(dataw[i] <=1552) $fwrite(fp, "%388h", tmp);
                else if(dataw[i] <=1568) $fwrite(fp, "%392h", tmp);
                else if(dataw[i] <=1584) $fwrite(fp, "%396h", tmp);
                else if(dataw[i] <=1600) $fwrite(fp, "%400h", tmp);
                else if(dataw[i] <=1616) $fwrite(fp, "%404h", tmp);
                else if(dataw[i] <=1632) $fwrite(fp, "%408h", tmp);
                else if(dataw[i] <=1648) $fwrite(fp, "%412h", tmp);
                else if(dataw[i] <=1664) $fwrite(fp, "%416h", tmp);
                else if(dataw[i] <=1680) $fwrite(fp, "%420h", tmp);
                else if(dataw[i] <=1696) $fwrite(fp, "%424h", tmp);
                else if(dataw[i] <=1712) $fwrite(fp, "%428h", tmp);
                else if(dataw[i] <=1728) $fwrite(fp, "%432h", tmp);
                else if(dataw[i] <=1744) $fwrite(fp, "%436h", tmp);
                else if(dataw[i] <=1760) $fwrite(fp, "%440h", tmp);
                else if(dataw[i] <=1776) $fwrite(fp, "%444h", tmp);
                else if(dataw[i] <=1792) $fwrite(fp, "%448h", tmp);
                else if(dataw[i] <=1808) $fwrite(fp, "%452h", tmp);
                else if(dataw[i] <=1824) $fwrite(fp, "%456h", tmp);
                else if(dataw[i] <=1840) $fwrite(fp, "%460h", tmp);
                else if(dataw[i] <=1856) $fwrite(fp, "%464h", tmp);
                else if(dataw[i] <=1872) $fwrite(fp, "%468h", tmp);
                else if(dataw[i] <=1888) $fwrite(fp, "%472h", tmp);
                else if(dataw[i] <=1904) $fwrite(fp, "%476h", tmp);
                else if(dataw[i] <=1920) $fwrite(fp, "%480h", tmp);
                else if(dataw[i] <=1936) $fwrite(fp, "%484h", tmp);
                else if(dataw[i] <=1952) $fwrite(fp, "%488h", tmp);
                else if(dataw[i] <=1968) $fwrite(fp, "%492h", tmp);
                else if(dataw[i] <=1984) $fwrite(fp, "%496h", tmp);
                else if(dataw[i] <=2000) $fwrite(fp, "%500h", tmp);
                else if(dataw[i] <=2016) $fwrite(fp, "%504h", tmp);
                else if(dataw[i] <=2032) $fwrite(fp, "%508h", tmp);
                else if(dataw[i] <=2048) $fwrite(fp, "%512h", tmp);

                if(i==dataw.size-1) $fwrite(fp, "\n");
                else                 $fwrite(fp, "\t");
            end
            $fclose(fp);
        end//end of if(fp)
    endfunction

    virtual function int get_next_field(string ss);
        int i;
        int state=0;
        for(i=0;i<ss.len;i++) begin
            //find the first field
            if( state==0&& (ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="f"||ss[i]>="A"&&ss[i]<="F") ) state=1;
            //continue to scan the first field
            if( state==1&& (ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="f"||ss[i]>="A"&&ss[i]<="F") ) state=1;
            //scan the first field done
            if( state==1&&!(ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="f"||ss[i]>="A"&&ss[i]<="F") ) state=2;
            //scan null letter after the first field
            if( state==2&&!(ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="f"||ss[i]>="A"&&ss[i]<="F") ) state=2;
            //find the second field
            if( state==2&& (ss[i]>="0"&&ss[i]<="9"||ss[i]>="a"&&ss[i]<="f"||ss[i]>="A"&&ss[i]<="F") ) return i;
        end
        return -1;
    endfunction

    virtual function int get_lines(ref longint clkcyc[string][$], ref logic[DATAW-1:0] data[$], string evtf[$], int dataw[$]);
        string line;
        logic[DATAW-1:0] tmp;
        logic[DATAW-1:0] datatmp;
        int shift;
        longint clkcyctmp;
        integer fp;
        get_lines = 0;
        clkcyc.delete();
        data.delete();
        if(this.mode!="tvread") return get_lines;
        fp = $fopen(FilePath, "r"); 
        if(fp==0) return get_lines;
        void'($fgets(line, fp)); //read head line
        while($fgets(line, fp)) begin
            get_lines++;
            datatmp = 0;
            shift = 0;
            foreach(evtf[i])  begin
                int aa;
                void'($sscanf(line, "%h", clkcyctmp));
                clkcyc[evtf[i]].push_back(clkcyctmp);
                aa = get_next_field(line);
                if(aa==-1&&i!=evtf.size-1) $display("Error: file:%s format is wrong at %t. TB file:%s line:%d", FilePath, $time, `__FILE__, `__LINE__ );
                line = line.substr(aa, line.len-1);
            end

            foreach(dataw[i]) begin
                int aa;
                void'($sscanf(line, "%h", tmp));
                datatmp += (tmp<<shift);
                shift+=dataw[i];
                aa = get_next_field(line);
                if(aa==-1&&i!=dataw.size-1) $display("Error: file:%s format is wrong at %t. TB file:%s line:%d", FilePath, $time, `__FILE__, `__LINE__ );
                line = line.substr(aa, line.len-1);
            end
            data.push_back(datatmp);
        end
        $fclose(fp);
    endfunction
endclass

class vld_rdy_data_trace_phy_mondrv#(int DATAW=8) extends trace_base_component;

    vld_rdy_data_trace_file_agent#(DATAW) FileAgent;
    virtual vld_rdy_data_trace_interface#(DATAW) VldRdyDataIf;
    
    string DataField[$];
    int    DataBitWidth[$];
    logic[DATAW-1:0] Data[$]; 
    logic[DATAW-1:0] RefData[$]; 
    logic[DATAW-1:0] MonChkData[$]; 

    vld_rdy_data#(DATAW) PutDataProc;
    data_base_object  PutData;

    string mode;
    string drvmode;
    
    string  EvtField[$];
    longint ClkCyc[string][$];
    longint RefClkCyc[string][$];
    longint MonChkClkCyc[string][$];
    
    
    function new(string name, string hier_name);
        super.new(name, hier_name);
        FileAgent = new();
        EvtField.push_back("vldevt");
        EvtField.push_back("rdyevt");
        PutDataProc = new();
        PutData = new();
    endfunction

    function void cfg(string mode, string drvmode);
        this.mode = mode;
        this.drvmode = drvmode;
    endfunction
    
    virtual function void file_agent_cfg();
        string filename;
        if(this.mode!="tvdump" && this.mode!="tvread") begin
             $display("ERROR: mode:%s is not tvdump or tvread in %s at line %d", this.mode, `__FILE__, `__LINE__);
        end
        filename = this.full_name();
        for(int i=0; i<filename.len; i++) 
            if(filename[i]==".") filename[i] = "_";

        PutDataProc.DataField    = this.DataField;
        PutDataProc.DataBitWidth = this.DataBitWidth;

        FileAgent.file_cfg({this.TVDir, "/", filename, "_phy.dat"}, this.mode);
        FileAgent.init_head_proc(EvtField, DataField);
        void'(FileAgent.get_lines(this.RefClkCyc, this.RefData, 
                            this.EvtField,  this.DataBitWidth));
        void'(FileAgent.get_lines(this.MonChkClkCyc, this.MonChkData, 
                            this.EvtField,  this.DataBitWidth));
    endfunction
    
    //this function usually should be overriden. PutDataProc is vld_rdy_data type of data
    //such data can be processed and form to more abstract data, PutData
    virtual function void abstract_put_data();
        PutData = PutDataProc;
    endfunction

    virtual function void evt_dump();
        bit ready=1;
        foreach(EvtField[i]) begin
            if(ClkCyc[EvtField[i]].size==0) begin
                ready = 0;
                break;
            end
        end
    
        if(Data.size>0&&ready) begin
//            foreach(EvtField[i]) begin
//$display("lineww, before_onelinedump %s evt size:%0d, first:%0h, %s at %t. TB file:%s line:%d", 
//                      EvtField[i],ClkCyc[EvtField[i]].size, ClkCyc[EvtField[i]][0], this.full_name(), $time, `__FILE__, `__LINE__ );
//            end
         //   FileAgent.dump_oneline(ClkCyc, Data[0], EvtField, DataBitWidth);
////////////////////////////////////////////////////////dump_oneline flat///////////////////////////////
    if(mode=="tvdump") begin
        integer fp;
        int shift = 0;
        logic[DATAW-1:0] tmp;
        logic[DATAW-1:0] shiftdata;

//$display("lineww, onelinedump to component:%s at %t. TB file:%s line:%d", this.full_name(), $time, `__FILE__, `__LINE__ );

        fp = $fopen(FileAgent.FilePath, "a"); 
        if(fp) begin
            foreach(EvtField[i]) begin
//$display("lineww, %s size:%0d, first:%0h, component:%s at %t. TB file:%s line:%d", 
//               EvtField[i], ClkCyc[EvtField[i]].size, ClkCyc[EvtField[i]][0],  this.full_name(), $time, `__FILE__, `__LINE__ );
                $fwrite(fp, "%16h\t", ClkCyc[EvtField[i]][0] );
            end
            shiftdata = Data[0];
            
            foreach(DataBitWidth[i]) begin
                tmp = shiftdata & ((1<<DataBitWidth[i])-1);
                shiftdata = shiftdata >> DataBitWidth[i];
                     if(DataBitWidth[i] <=  16) $fwrite(fp, "%4h", tmp);
                else if(DataBitWidth[i] <=  32) $fwrite(fp, "%8h", tmp);
                else if(DataBitWidth[i] <=  48) $fwrite(fp, "%12h", tmp);
                else if(DataBitWidth[i] <=  64) $fwrite(fp, "%16h", tmp);
                else if(DataBitWidth[i] <=  80) $fwrite(fp, "%20h", tmp);
                else if(DataBitWidth[i] <=  96) $fwrite(fp, "%24h", tmp);
                else if(DataBitWidth[i] <= 112) $fwrite(fp, "%28h", tmp);
                else if(DataBitWidth[i] <= 128) $fwrite(fp, "%32h", tmp);
                else if(DataBitWidth[i] <= 144) $fwrite(fp, "%36h", tmp);
                else if(DataBitWidth[i] <= 160) $fwrite(fp, "%40h", tmp);
                else if(DataBitWidth[i] <= 176) $fwrite(fp, "%44h", tmp);
                else if(DataBitWidth[i] <= 192) $fwrite(fp, "%48h", tmp);
                else if(DataBitWidth[i] <= 208) $fwrite(fp, "%52h", tmp);
                else if(DataBitWidth[i] <= 224) $fwrite(fp, "%56h", tmp);
                else if(DataBitWidth[i] <= 240) $fwrite(fp, "%60h", tmp);
                else if(DataBitWidth[i] <= 256) $fwrite(fp, "%64h", tmp);
                else if(DataBitWidth[i] <= 272) $fwrite(fp, "%68h", tmp);
                else if(DataBitWidth[i] <= 288) $fwrite(fp, "%72h", tmp);
                else if(DataBitWidth[i] <= 304) $fwrite(fp, "%76h", tmp);
                else if(DataBitWidth[i] <= 320) $fwrite(fp, "%80h", tmp);
                else if(DataBitWidth[i] <= 336) $fwrite(fp, "%84h", tmp);
                else if(DataBitWidth[i] <= 352) $fwrite(fp, "%88h", tmp);
                else if(DataBitWidth[i] <= 368) $fwrite(fp, "%92h", tmp);
                else if(DataBitWidth[i] <= 384) $fwrite(fp, "%96h", tmp);
                else if(DataBitWidth[i] <= 400) $fwrite(fp, "%100h", tmp);
                else if(DataBitWidth[i] <= 416) $fwrite(fp, "%104h", tmp);
                else if(DataBitWidth[i] <= 432) $fwrite(fp, "%108h", tmp);
                else if(DataBitWidth[i] <= 448) $fwrite(fp, "%112h", tmp);
                else if(DataBitWidth[i] <= 464) $fwrite(fp, "%116h", tmp);
                else if(DataBitWidth[i] <= 480) $fwrite(fp, "%120h", tmp);
                else if(DataBitWidth[i] <= 496) $fwrite(fp, "%124h", tmp);
                else if(DataBitWidth[i] <= 512) $fwrite(fp, "%128h", tmp);
                else if(DataBitWidth[i] <= 528) $fwrite(fp, "%132h", tmp);
                else if(DataBitWidth[i] <= 544) $fwrite(fp, "%136h", tmp);
                else if(DataBitWidth[i] <= 560) $fwrite(fp, "%140h", tmp);
                else if(DataBitWidth[i] <= 576) $fwrite(fp, "%144h", tmp);
                else if(DataBitWidth[i] <= 592) $fwrite(fp, "%148h", tmp);
                else if(DataBitWidth[i] <= 608) $fwrite(fp, "%152h", tmp);
                else if(DataBitWidth[i] <= 624) $fwrite(fp, "%156h", tmp);
                else if(DataBitWidth[i] <= 640) $fwrite(fp, "%160h", tmp);
                else if(DataBitWidth[i] <= 656) $fwrite(fp, "%164h", tmp);
                else if(DataBitWidth[i] <= 672) $fwrite(fp, "%168h", tmp);
                else if(DataBitWidth[i] <= 688) $fwrite(fp, "%172h", tmp);
                else if(DataBitWidth[i] <= 704) $fwrite(fp, "%176h", tmp);
                else if(DataBitWidth[i] <= 720) $fwrite(fp, "%180h", tmp);
                else if(DataBitWidth[i] <= 736) $fwrite(fp, "%184h", tmp);
                else if(DataBitWidth[i] <= 752) $fwrite(fp, "%188h", tmp);
                else if(DataBitWidth[i] <= 768) $fwrite(fp, "%192h", tmp);
                else if(DataBitWidth[i] <= 784) $fwrite(fp, "%196h", tmp);
                else if(DataBitWidth[i] <= 800) $fwrite(fp, "%200h", tmp);
                else if(DataBitWidth[i] <= 816) $fwrite(fp, "%204h", tmp);
                else if(DataBitWidth[i] <= 832) $fwrite(fp, "%208h", tmp);
                else if(DataBitWidth[i] <= 848) $fwrite(fp, "%212h", tmp);
                else if(DataBitWidth[i] <= 864) $fwrite(fp, "%216h", tmp);
                else if(DataBitWidth[i] <= 880) $fwrite(fp, "%220h", tmp);
                else if(DataBitWidth[i] <= 896) $fwrite(fp, "%224h", tmp);
                else if(DataBitWidth[i] <= 912) $fwrite(fp, "%228h", tmp);
                else if(DataBitWidth[i] <= 928) $fwrite(fp, "%232h", tmp);
                else if(DataBitWidth[i] <= 944) $fwrite(fp, "%236h", tmp);
                else if(DataBitWidth[i] <= 960) $fwrite(fp, "%240h", tmp);
                else if(DataBitWidth[i] <= 976) $fwrite(fp, "%244h", tmp);
                else if(DataBitWidth[i] <= 992) $fwrite(fp, "%248h", tmp);
                else if(DataBitWidth[i] <=1008) $fwrite(fp, "%252h", tmp);
                else if(DataBitWidth[i] <=1024) $fwrite(fp, "%256h", tmp);
                else if(DataBitWidth[i] <=1040) $fwrite(fp, "%260h", tmp);
                else if(DataBitWidth[i] <=1056) $fwrite(fp, "%264h", tmp);
                else if(DataBitWidth[i] <=1072) $fwrite(fp, "%268h", tmp);
                else if(DataBitWidth[i] <=1088) $fwrite(fp, "%272h", tmp);
                else if(DataBitWidth[i] <=1104) $fwrite(fp, "%276h", tmp);
                else if(DataBitWidth[i] <=1120) $fwrite(fp, "%280h", tmp);
                else if(DataBitWidth[i] <=1136) $fwrite(fp, "%284h", tmp);
                else if(DataBitWidth[i] <=1152) $fwrite(fp, "%288h", tmp);
                else if(DataBitWidth[i] <=1168) $fwrite(fp, "%292h", tmp);
                else if(DataBitWidth[i] <=1184) $fwrite(fp, "%296h", tmp);
                else if(DataBitWidth[i] <=1200) $fwrite(fp, "%300h", tmp);
                else if(DataBitWidth[i] <=1216) $fwrite(fp, "%304h", tmp);
                else if(DataBitWidth[i] <=1232) $fwrite(fp, "%308h", tmp);
                else if(DataBitWidth[i] <=1248) $fwrite(fp, "%312h", tmp);
                else if(DataBitWidth[i] <=1264) $fwrite(fp, "%316h", tmp);
                else if(DataBitWidth[i] <=1280) $fwrite(fp, "%320h", tmp);
                else if(DataBitWidth[i] <=1296) $fwrite(fp, "%324h", tmp);
                else if(DataBitWidth[i] <=1312) $fwrite(fp, "%328h", tmp);
                else if(DataBitWidth[i] <=1328) $fwrite(fp, "%332h", tmp);
                else if(DataBitWidth[i] <=1344) $fwrite(fp, "%336h", tmp);
                else if(DataBitWidth[i] <=1360) $fwrite(fp, "%340h", tmp);
                else if(DataBitWidth[i] <=1376) $fwrite(fp, "%344h", tmp);
                else if(DataBitWidth[i] <=1392) $fwrite(fp, "%348h", tmp);
                else if(DataBitWidth[i] <=1408) $fwrite(fp, "%352h", tmp);
                else if(DataBitWidth[i] <=1424) $fwrite(fp, "%356h", tmp);
                else if(DataBitWidth[i] <=1440) $fwrite(fp, "%360h", tmp);
                else if(DataBitWidth[i] <=1456) $fwrite(fp, "%364h", tmp);
                else if(DataBitWidth[i] <=1472) $fwrite(fp, "%368h", tmp);
                else if(DataBitWidth[i] <=1488) $fwrite(fp, "%372h", tmp);
                else if(DataBitWidth[i] <=1504) $fwrite(fp, "%376h", tmp);
                else if(DataBitWidth[i] <=1520) $fwrite(fp, "%380h", tmp);
                else if(DataBitWidth[i] <=1536) $fwrite(fp, "%384h", tmp);
                else if(DataBitWidth[i] <=1552) $fwrite(fp, "%388h", tmp);
                else if(DataBitWidth[i] <=1568) $fwrite(fp, "%392h", tmp);
                else if(DataBitWidth[i] <=1584) $fwrite(fp, "%396h", tmp);
                else if(DataBitWidth[i] <=1600) $fwrite(fp, "%400h", tmp);
                else if(DataBitWidth[i] <=1616) $fwrite(fp, "%404h", tmp);
                else if(DataBitWidth[i] <=1632) $fwrite(fp, "%408h", tmp);
                else if(DataBitWidth[i] <=1648) $fwrite(fp, "%412h", tmp);
                else if(DataBitWidth[i] <=1664) $fwrite(fp, "%416h", tmp);
                else if(DataBitWidth[i] <=1680) $fwrite(fp, "%420h", tmp);
                else if(DataBitWidth[i] <=1696) $fwrite(fp, "%424h", tmp);
                else if(DataBitWidth[i] <=1712) $fwrite(fp, "%428h", tmp);
                else if(DataBitWidth[i] <=1728) $fwrite(fp, "%432h", tmp);
                else if(DataBitWidth[i] <=1744) $fwrite(fp, "%436h", tmp);
                else if(DataBitWidth[i] <=1760) $fwrite(fp, "%440h", tmp);
                else if(DataBitWidth[i] <=1776) $fwrite(fp, "%444h", tmp);
                else if(DataBitWidth[i] <=1792) $fwrite(fp, "%448h", tmp);
                else if(DataBitWidth[i] <=1808) $fwrite(fp, "%452h", tmp);
                else if(DataBitWidth[i] <=1824) $fwrite(fp, "%456h", tmp);
                else if(DataBitWidth[i] <=1840) $fwrite(fp, "%460h", tmp);
                else if(DataBitWidth[i] <=1856) $fwrite(fp, "%464h", tmp);
                else if(DataBitWidth[i] <=1872) $fwrite(fp, "%468h", tmp);
                else if(DataBitWidth[i] <=1888) $fwrite(fp, "%472h", tmp);
                else if(DataBitWidth[i] <=1904) $fwrite(fp, "%476h", tmp);
                else if(DataBitWidth[i] <=1920) $fwrite(fp, "%480h", tmp);
                else if(DataBitWidth[i] <=1936) $fwrite(fp, "%484h", tmp);
                else if(DataBitWidth[i] <=1952) $fwrite(fp, "%488h", tmp);
                else if(DataBitWidth[i] <=1968) $fwrite(fp, "%492h", tmp);
                else if(DataBitWidth[i] <=1984) $fwrite(fp, "%496h", tmp);
                else if(DataBitWidth[i] <=2000) $fwrite(fp, "%500h", tmp);
                else if(DataBitWidth[i] <=2016) $fwrite(fp, "%504h", tmp);
                else if(DataBitWidth[i] <=2032) $fwrite(fp, "%508h", tmp);
                else if(DataBitWidth[i] <=2048) $fwrite(fp, "%512h", tmp);

                if(i==DataBitWidth.size-1) $fwrite(fp, "\n");
                else                 $fwrite(fp, "\t");
            end
            $fclose(fp);
        end//end of if(fp)
    end
////////////////////////////////////////////////////////dump_oneline flat end///////////////////////////////

            PutDataProc.update_data(Data[0]);
            abstract_put_data();
            put_data(this.PutData, "mon_data");
            void'(Data.pop_front());
            foreach(EvtField[i]) begin
//$display("lineww, after_onelinedump %s evt size:%0d, first:%0h, %s at %t. TB file:%s line:%d", 
//                      EvtField[i],ClkCyc[EvtField[i]].size, ClkCyc[EvtField[i]][0], this.full_name(), $time, `__FILE__, `__LINE__ );
                void'(ClkCyc[EvtField[i]].pop_front());
            end
        end
    endfunction

    virtual task mon_task();
        bit[1:0] state;
        int rdymiss = 0;
        int vldmiss = 0;
        int datamiss = 0;
        state = 0;
        forever begin
            @(posedge VldRdyDataIf.clk);
            if(VldRdyDataIf.valid===1) begin
                if((state&1)==0) begin
                    state[0]=1;
                    ClkCyc["vldevt"].push_back(VldRdyDataIf.clkcnt);
                    if(mode=="tvread" && vldmiss<`ERRCNT && MonChkClkCyc["vldevt"][0]!=ClkCyc["vldevt"][$]) begin
                        $display("COSIM_TRACE_DRV WARNING: %s detects dut vld timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                             this.full_name(), $time, `__FILE__, `__LINE__);
                        vldmiss++;
                    end
                    void'(MonChkClkCyc["vldevt"].pop_front());
                end
            end
            if(VldRdyDataIf.valid===0) begin
                if(state[0]==1) begin
                    $display("ERROR: %s detects valid change violation at %t. TB file:%s line:%d", 
                         this.full_name(), $time, `__FILE__, `__LINE__);
                    state[0]=0;
                end
            end
    
            if(VldRdyDataIf.ready===1) begin
                if(state[1]==0) begin
                    state[1]=1;
                    ClkCyc["rdyevt"].push_back(VldRdyDataIf.clkcnt);
                    //ignore thie first ready check as it may be asserted before reset
                    if(mode=="tvread" && rdymiss<`ERRCNT+1 && MonChkClkCyc["rdyevt"][0]!=ClkCyc["rdyevt"][$]&&rdymiss>0) begin
                        $display("COSIM_TRACE_DRV WARNING: %s detects dut rdy timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                             this.full_name(), $time, `__FILE__, `__LINE__);
                        rdymiss++;
                    end
                    void'(MonChkClkCyc["rdyevt"].pop_front());
                end
            end
            if(VldRdyDataIf.ready===0) begin
                if(state[1]==1) begin
                    $display("ERROR %s detects ready change violation at %t. TB file:%s line:%d", 
                         this.full_name(), $time, `__FILE__, `__LINE__);
                    state[1]=0;
                end
            end
            if(state==3) begin
                Data.push_back(VldRdyDataIf.data);
                if(mode=="tvread" && datamiss<`ERRCNT && MonChkData[0]!=Data[$]) begin
                    $display("COSIM_TRACE_DRV WARNING: %s detects dut data changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                         this.full_name(), $time, `__FILE__, `__LINE__);
                    datamiss++;
                end
                state = 0;
                void'(MonChkData.pop_front());
                evt_dump();
            end
        end
    endtask

    virtual task rdydrv_task();
        int delay;
        int mismat_rdycnt = 0;
        int mismat_vldcnt = 0;
        int delay_cmp;
        longint sum_delay;
        longint avg_cnt;
        if( !(this.drvmode == "rdydrv" && this.mode =="tvread") ) wait(0);
        if(RefData.size) begin
            avg_cnt = 1;
            sum_delay = ( (RefClkCyc["rdyevt"][0] - RefClkCyc["vldevt"][0])>=0 ? (RefClkCyc["rdyevt"][0] - RefClkCyc["vldevt"][0]):-1 );
        end
        VldRdyDataIf.ready = 0;
        delay = 0;
        forever begin
            @(posedge VldRdyDataIf.clk);
            delay_cmp = 0;
            if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===1) begin
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.data!=RefData[0]) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut output data mismatch at %t, compared with HW_EMU. TB file:%s line:%d. Ref:0x%0h, Dut:0x%0h", 
                //         this.full_name(), $time, `__FILE__, `__LINE__, RefData[0], VldRdyDataIf.data);
                //    mismat_rdycnt++;
                //end
                //if(RefData.size>0 && mismat_vldcnt<5 && VldRdyDataIf.clkcnt!=RefClkCyc["vldevt"][0]&&delay==0) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut valid timing changed at %t, compared with HW_EMU. TB file:%s line:%d.", 
                //        this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                //if(mismat_rdycnt<5&&RefData.size==0) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut output more data at %t, compared with HW_EMU. TB file:%s line:%d.", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                RefData.pop_front();
                foreach(EvtField[i]) RefClkCyc[EvtField[i]].pop_front();
                if(RefData.size) begin
                    avg_cnt++;
                    sum_delay += ( (RefClkCyc["rdyevt"][0] - RefClkCyc["vldevt"][0])>=0 ? (RefClkCyc["rdyevt"][0] - RefClkCyc["vldevt"][0]):-1 );
                end
                #`DRV_DELAY; VldRdyDataIf.ready = 0;
                delay = 0;
                delay_cmp=1;
            end

            if(RefData.size>0 && RefClkCyc["vldevt"][0]>=RefClkCyc["rdyevt"][0]||
               RefData.size==0&&avg_cnt>0&&(sum_delay/avg_cnt)<=0||
               RefData.size==0&&avg_cnt==0) begin
                if( (RefData.size>0) ? (RefClkCyc["rdyevt"][0]-1<=VldRdyDataIf.clkcnt):(avg_cnt>0 && sum_delay/avg_cnt<0) ) begin
                    #`DRV_DELAY; VldRdyDataIf.ready = 1;
                end else begin
                    @(negedge VldRdyDataIf.clk);
                    if(VldRdyDataIf.valid === 1) 
                        VldRdyDataIf.ready = 1;
                end
                delay = 0;
            end
            if( RefData.size>0 && RefClkCyc["vldevt"][0]<RefClkCyc["rdyevt"][0]||avg_cnt>0&&RefData.size==0&&(sum_delay/avg_cnt)>0 ) begin
                if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===0) begin
                    //if(RefData.size>0 && mismat_vldcnt<5 && VldRdyDataIf.clkcnt!=RefClkCyc["vldevt"][0]&&delay==0) begin
                    //    $display("COSIM_TRACE_DRV WARNING: %s detects dut valid timing changed at %t, compared with HW_EMU. TB file:%s line:%d.", 
                    //        this.full_name(), $time, `__FILE__, `__LINE__);
                    //    mismat_vldcnt++;
                    //end
                    delay = delay+1-delay_cmp;
                end
                if( delay == ((RefData.size>0) ? (RefClkCyc["rdyevt"][0]-RefClkCyc["vldevt"][0]):(sum_delay/avg_cnt)) ) begin
                    #`DRV_DELAY; VldRdyDataIf.ready = 1;
                end
            end
        end
    endtask

    virtual task vlddrv_task();
        int mismat_rdycnt = 0;
        int mismat_vldcnt = 0;
        if(this.drvmode != "vlddrv" || this.mode !="tvread") wait(0);
        //int state = 0;
        VldRdyDataIf.valid = 0;
        while(RefData.size) begin
            @(posedge VldRdyDataIf.clk);
            if(VldRdyDataIf.valid===1&&VldRdyDataIf.ready===1) begin
                //if(mismat_rdycnt<5 && RefData.size>0 && VldRdyDataIf.clkcnt!=RefClkCyc["rdyevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_rdycnt++;
                //end
                RefData.pop_front();
                foreach(EvtField[i]) RefClkCyc[EvtField[i]].pop_front();
                #`DRV_DELAY; VldRdyDataIf.valid = 0;
            end
            if(RefData.size>0 && VldRdyDataIf.clkcnt>=RefClkCyc["vldevt"][0]-1) begin
                //state = 1;
                //if(mismat_vldcnt<5 && VldRdyDataIf.clkcnt!=RefClkCyc["vldevt"][0]-1) begin
                //    $display("COSIM_TRACE_DRV WARNING: %s detects dut ready timing changed at %t, compared with HW_EMU. TB file:%s line:%d", 
                //         this.full_name(), $time, `__FILE__, `__LINE__);
                //    mismat_vldcnt++;
                //end
                #`DRV_DELAY; VldRdyDataIf.valid = 1;
                VldRdyDataIf.data = RefData[0];
            end
            //if(state==0) VldRdyDataIf.valid = 0;
        end

    endtask
    
    virtual task run();
        this.file_agent_cfg();
        // if(mode=="tvdump") VldRdyDataIf.fp = FileAgent.fp; //close file is done by IF
        fork
            forever begin
                wait(VldRdyDataIf.rst===1);
                fork
                    mon_task();
                    vlddrv_task();
                    rdydrv_task();
                    begin
                        wait(VldRdyDataIf.rst==0);
                    end
                join_any
                disable fork;
            end //end of reset re-entry
        join
    endtask
endclass
`endif
