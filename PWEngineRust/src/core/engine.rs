use std::{cell::RefCell, collections::HashMap, ffi::CStr, rc::Rc};

use sdl3_sys::{error::SDL_GetError, gpu::SDL_CreateGPUDeviceWithProperties, hints::{SDL_HINT_GPU_DRIVER, SDL_SetHint}, properties::{SDL_CreateProperties, SDL_DestroyProperties, SDL_SetBooleanProperty}, render::{SDL_RenderClear, SDL_RenderPresent, SDL_SetRenderDrawColor}, *};
use crate::{core::{scene::PWEScene, window::{PWEWindow}}, utils::log::*};
pub struct PWEngine
{
    device: Rc<RefCell<*mut gpu::SDL_GPUDevice>>,
    window_map: HashMap<String, Rc<RefCell<PWEWindow>>>,
    scene_map: HashMap<String, Rc<RefCell<dyn PWEScene>>>,
    curr_window: Option<*mut PWEWindow>,
    curr_scene: Option<*mut dyn PWEScene>
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
            init::SDL_Init(init::SDL_INIT_VIDEO);
            sdl3_ttf_sys::ttf::TTF_Init();
            //Self::engine_info();
            let device_ptr = gpu::SDL_CreateGPUDevice(gpu::SDL_GPU_SHADERFORMAT_SPIRV,false, c"vulkan".as_ptr());
            if device_ptr.is_null()
            {
                let c_ptr_error = SDL_GetError();
                let c_error = CStr::from_ptr(c_ptr_error).to_owned();
                let error = c_error.to_str().unwrap();
                PWELog::log_error(format!("Create device failed. SDL_ERROR:{}", error ).as_str());
                panic!("Create device failed.")
            }
            let device = Rc::new(RefCell::new(device_ptr));
            return PWEngine 
            {
                device: device,
                window_map: HashMap::new(),
                scene_map: HashMap::new(),
                curr_window: None,
                curr_scene: None
            };
        }
        
    }
    pub fn registry_window(&mut self, window_name: &str)
    {
        let name = String::from(window_name);
        let window = Rc::new(RefCell::new(PWEWindow::builder(&name, Rc::clone(&self.device))));
        self.window_map.insert(name, Rc::clone(&window));

    }
    pub fn get_window(&self, name: &str) -> Rc<RefCell<PWEWindow>>
    {
        let window = self.window_map.get(name).unwrap();
        
        return Rc::clone(window);
    }
    pub fn registry_scene<T>(&mut self, scene_name: &str, scene: T)
    where T: PWEScene + 'static
    {
        let name = String::from(scene_name);
        self.scene_map.insert(name, Rc::new(RefCell::new(scene)));
    }
    pub fn binding_window(&mut self, window_name: &str)
    {
        match self.window_map.get(window_name) {
            Some(window) => {
                self.curr_window = Some(window.as_ptr());
            },
            None => {
                PWELog::log_warn("Can not found window");
                return;
            }
        }
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
                let window = self.curr_window.unwrap();
                let render = (*window).get_render_ptr();
                SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
                SDL_RenderClear(render);
                SDL_RenderPresent(render);
                // let scene = self.curr_scene.unwrap();
                // (*scene).render(render);
            }
        }
    }
}
