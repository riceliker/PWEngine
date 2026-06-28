#![allow(non_upper_case_globals, non_camel_case_types, unused)]
#[cfg(not(rust_analyzer))]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(rust_analyzer)]
mod bindings {
    #[repr(C)]
    pub struct PWEngineCAPI_Engine {
        _priv: [u8; 0],
    }
    extern "C" {
        pub fn PWEngineCAPI_CreateEngine() -> *mut PWEngineCAPI_Engine;
        pub fn PWEngineCAPI_DestroyEngine(_: *mut PWEngineCAPI_Engine);
    }
}
#[cfg(rust_analyzer)]
use bindings::*;

fn main() {
    unsafe {
        let eng = PWEngineCAPI_CreateEngine();
        if eng.is_null() {
            eprintln!("引擎初始化失败");
            return;
        }
        println!("Rust 调用C引擎成功");

        PWEngineCAPI_DestroyEngine(eng);
    }
}
