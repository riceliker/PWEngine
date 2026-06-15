mod core;
mod utils;
use sdl3::sys::events::{SDL_EVENT_QUIT, SDL_Event, SDL_PollEvent};

use crate::core::window::{Window, WindowInfo};
use crate::utils::vec::Vec2;
fn main() 
{
    let mut win = Window::new();
    win.create_window(WindowInfo {
        logical_resolution: Vec2 {x: 1280,y: 720}
    });
    println!("Hello, world!");
    let mut is_running = true;
    unsafe 
    {
        while(is_running)
        {
            let mut event = SDL_Event::default();
            while (SDL_PollEvent(&mut event)) 
            {
                if(event.r#type == SDL_EVENT_QUIT) 
                {
                    is_running = false;
                }
            }
        }
    }
}
