use std::{cell::RefCell, collections::HashMap, ffi::CStr, rc::Rc};

use sdl3_sys::*;
use crate::{core::window::{self, PWEWindow}, utils::log::*};
pub struct PWEngine
{
    device: Rc<RefCell<*mut gpu::SDL_GPUDevice>>,
    window_map: HashMap<String, Rc<RefCell<PWEWindow>>>
}
impl PWEngine
{
    pub fn engine_info()
    {
        unsafe 
        {
            let gpu_count = gpu::SDL_GetNumGPUDrivers();
            let mut info_msg: String = String::new();
            let mut i = 0;
            while i < gpu_count
            {
                let gpu_name_ptr = gpu::SDL_GetGPUDriver(i);
                if gpu_name_ptr.is_null()
                {
                    continue;
                }
                let gpu_name = CStr::from_ptr(gpu_name_ptr).to_str().unwrap();
                if i == 0
                {
                    info_msg = format!("{}",gpu_name);
                }
                else 
                {
                    info_msg = format!("{},{}", info_msg, gpu_name);
                }
                i += 1;
            }

            PWELog::log_info("PWEngine v0.1.0 (c) 2026-present riceliker & Contributors. Open Source License: LGPL2.0.");
            PWELog::log_info("Device Info:");
            PWELog::log_info(&format!("Graphic API:{} ", info_msg));
        }
    }
    pub fn create_engine() -> Self
    {
        unsafe 
        {
            let init_code = init::SDL_Init(init::SDL_INIT_VIDEO);
            if init_code
            {
                PWELog::log_info("SDL has init.");
            }
            else 
            {
                PWELog::log_error("ERROR: SDL init failed");
            }
            Self::engine_info();
            let device_ptr = gpu::SDL_CreateGPUDevice(gpu::SDL_GPU_SHADERFORMAT_SPIRV,false, c"vulkan".as_ptr());
            if device_ptr.is_null()
            {
                PWELog::log_error("Create device failed.");
                panic!("Create device failed.")
            }
            let device = Rc::new(RefCell::new(device_ptr));
            return PWEngine 
            {
                device: device,
                window_map: HashMap::new()
            };
        }
        
    }
    pub fn create_window(&mut self, window_name: &str) -> Rc<RefCell<PWEWindow>>
    {
        let name = String::from(window_name);
        let window = Rc::new(RefCell::new(PWEWindow::builder(&name, Rc::clone(&self.device))));
        self.window_map.insert(name, Rc::clone(&window));
        return window;
    }
    pub fn show_window(&self, name: &str) -> Rc<RefCell<PWEWindow>>
    {
        let window = self.window_map.get(name).unwrap();
        unsafe 
        {
            video::SDL_ShowWindow(window.borrow_mut().get_window_ptr());
            video::SDL_SetWindowPosition(window.borrow_mut().get_window_ptr(),  video::SDL_WINDOWPOS_CENTERED, video::SDL_WINDOWPOS_CENTERED);
        }
        return Rc::clone(window);
    }
    pub fn run(&self) 
    {
        unsafe
        {
            let mut event: sdl3_sys::events::SDL_Event = Default::default();
            let mut is_running = true;
            while is_running 
            {
                while sdl3_sys::events::SDL_PollEvent(&mut event)
                {
                    if event.r#type == sdl3_sys::events::SDL_EVENT_QUIT
                    {
                        is_running = false;
                    }
                }
            }
        }
    }
}
