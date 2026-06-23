use std::ffi::{CString, c_char};

use sdl3_sys::log::{SDL_LogError, SDL_LogInfo, SDL_LogWarn};

pub struct PWELog;

impl PWELog 
{
    pub fn str_c(msg: &str) -> *const c_char
    {
        let cstr = CString::new(msg).unwrap();
        return cstr.as_ptr();
    }
    pub fn string_c(msg: &String) -> *const c_char
    {
        let s = msg.as_str();
        let cstr = CString::new(s).unwrap();
        return cstr.as_ptr();
    }
    pub fn log_info(info: &str)
    {
        unsafe 
        {
            let cstr = CString::new(info).unwrap();
            SDL_LogInfo(0, cstr.as_ptr());
        }
        
    }
    pub fn log_warn(info: &str)
    {
        unsafe 
        {
            let cstr = CString::new(info).unwrap();
            SDL_LogWarn(1, cstr.as_ptr());
        }
    }
    pub fn log_error(info: &str)
    {
        unsafe 
        {
            let cstr = CString::new(info).unwrap();
            SDL_LogError(1, cstr.as_ptr());
            panic!("{}", info);
        }
    }
    pub fn err_null<T>(ptr: *const T, warn_msg: &str) -> *const T
    {
        unsafe 
        {
            if ptr.is_null()
            {
                SDL_LogWarn(0, Self::str_c(warn_msg));
            }
            return ptr;
        }
    }
}