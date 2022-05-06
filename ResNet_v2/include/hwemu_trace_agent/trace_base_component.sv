`ifndef TRACE_BASE_COMPONENT__SV
`define TRACE_BASE_COMPONENT__SV
class data_base_object;
endclass

class trace_base_component;

string inst_name;
string hier_name;
static string TVDir;

trace_base_component puts[$];
trace_base_component gets[$];
trace_base_component hooks[$];

function new(string inst_name, string hier_name);
    this.inst_name = inst_name;
    this.hier_name = hier_name;
endfunction

function string full_name();
    return {hier_name, ".", inst_name};
endfunction

function void add_puts(trace_base_component putc);
    puts.push_back(putc);
endfunction
function void remove_puts(trace_base_component putc);
    for(int i=0; i<puts.size; ) begin
        if( puts[i] == putc ) begin
            puts.delete(i);
        end else i++;
    end
endfunction

function void add_gets(trace_base_component getc);
    gets.push_back(getc);
endfunction
function void remove_gets(trace_base_component getc);
    for(int i=0; i<gets.size; ) begin
        if( gets[i] == getc ) begin
            gets.delete(i);
        end else i++;
    end
endfunction

function void add_hooks(trace_base_component hookc);
    hooks.push_back(hookc);
endfunction
function void remove_hooks(trace_base_component hookc);
    for(int i=0; i<hooks.size; ) begin
        if( hooks[i] == hookc ) begin
            hooks.delete(i);
        end else i++;
    end
endfunction

virtual function void put_data(data_base_object data, string flag);
    foreach(puts[i]) puts[i].put_data_called(data, flag);
endfunction
virtual function void put_data_called(data_base_object data, string flag);
endfunction

virtual function void get_data(data_base_object data, string flag);
    foreach(gets[i]) gets[i].get_data_called(data, flag);
endfunction
virtual function void get_data_called(data_base_object data, string flag);
endfunction

virtual function void hook_data(data_base_object data, string flag);
    foreach(hooks[i]) hooks[i].hook_data_called(data, flag);
endfunction
virtual function void hook_data_called(data_base_object data, string flag);
endfunction

virtual task run();
endtask
endclass
`endif
