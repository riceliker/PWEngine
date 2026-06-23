use std::{any::Any, ptr::null};

use crate::{core::{engine::PWEngine, window::{self, PWEWindow}}, utils::vec::Vec2};


mod core;
mod utils;
fn main() 
{
    let mut engine = PWEngine::create_engine();
    let mut window = engine.create_window("Main");

    window.borrow_mut().create_window(Vec2{x:1280, y:720} , window::PWEWindowMode::RESIZABLE);
    window.borrow_mut().create_render(Vec2 {x: 1280, y: 720});

    engine.show_window("Main");

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
                    let render = window.borrow_mut().get_render_ptr();
                    sdl3_sys::render::SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
                    sdl3_sys::render::SDL_RenderClear(render);
                    sdl3_sys::render::SDL_RenderPresent(render);   
                }
            }
        }
    

}
