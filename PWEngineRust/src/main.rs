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
        pub fn PWEngineCAPI_TestEngineSDLRender();
    }
}
#[cfg(rust_analyzer)]
use bindings::*;

fn main() {
    unsafe {
        PWEngineCAPI_TestEngineSDLRender();
    }
}
