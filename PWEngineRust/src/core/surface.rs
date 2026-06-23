use std::{cell::RefCell, rc::Rc};

use sdl3_sys::surface::SDL_Surface;


struct PWESurface
{
    surface: Option<Rc<RefCell<*mut SDL_Surface>>>
}

impl PWESurface
{
    pub fn create_from_png(path: String)
    {

    }

}