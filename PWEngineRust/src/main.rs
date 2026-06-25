use std::{any::Any, ptr::null};

use crate::{core::{engine::PWEngine, scene::PWECanvasScene, surface::PWESurfacePool, window::{self, PWEWindow}}, utils::vec::Vec2};


mod core;
mod utils;
fn main() 
{
    let mut engine = PWEngine::create_engine();

    engine.registry_window("Main");
    engine.get_window("Main").borrow_mut().create_window(Vec2{x:1280, y:720} , window::PWEWindowMode::RESIZABLE);
    engine.get_window("Main").borrow_mut().create_render(Vec2 {x: 1280, y: 720});

    // let mut surface_pool = PWESurfacePool::new();
    // surface_pool.add_surface_by_image("hello", "icon128.png");

    // let mut canvas = PWECanvasScene::new();
    // canvas.add_surface("Test", 0, surface_pool.get_surface("hello").unwrap());

    // engine.registry_scene("Scene", canvas);

    engine.binding_window("Main");

    engine.run();
    

}
