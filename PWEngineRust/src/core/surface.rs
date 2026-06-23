
use std::ptr;

use sdl3_sys::surface::{SDL_Surface};


pub struct PWESurface
{
    surface: *mut SDL_Surface,
}

impl PWESurface
{
    pub fn new(surface: *mut SDL_Surface) -> PWESurface
    {
        return PWESurface 
        { 
            surface : surface
        };
    }
    pub fn duplicate(&self) -> Self {
        let new_surf = if self.surface.is_null() {
            std::ptr::null_mut()
        } else {
            unsafe { sdl3_sys::surface::SDL_DuplicateSurface(self.surface) }
        };
        Self { surface: new_surf }
    }
}
