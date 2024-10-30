#[macro_export]
macro_rules! app_modules {
    ($(
        $module_name:ident {$($event:ident),* $(,)? }
    ),* $(,)?) => {
        enum ModuleEvent {
            MainLoopLoad,
            MainGameLoad,
            Tick,
            Draw,
        }

        pub static APP_CONTEXT: once_cell::sync::Lazy<critical_section::Mutex<AppContext>> =
            once_cell::sync::Lazy::new(|| critical_section::Mutex::new(AppContext::new()));

        paste::paste! {
            pub struct AppContext {
                pub process_inputs_hook: core::cell::RefCell<ProcessInputsHook>,
                pub draw_debug_text_hook: core::cell::RefCell<DrawDebugTextHook>,
                pub oslink_hook: core::cell::RefCell<OSLinkHook>,

                $(
                pub [<$module_name:snake>]: core::cell::RefCell<$module_name>,
                )*
            }

            impl AppContext {
                fn new() -> Self {
                    $crate::systems::heap::HEAP.init();

                    // Check for valid module events
                    $(
                        $(
                            let _ = ModuleEvent::$event;
                        )*
                    )*

                    Self {
                        process_inputs_hook: core::cell::RefCell::new(ProcessInputsHook::new()),
                        draw_debug_text_hook: core::cell::RefCell::new(DrawDebugTextHook::new()),
                        oslink_hook: core::cell::RefCell::new(OSLinkHook::new()),

                        $(
                        [<$module_name:snake>]: core::cell::RefCell::new($module_name::new()),
                        )*
                    }
                }

                fn init(&self) {
                    // Generate calls to on_main_loop_load functions
                    $(
                        $crate::app_modules!(@main_loop_load $module_name {$($event),*} self);
                    )*
                }
            }

            // Tick functions hook
            $crate::hook!(ProcessInputsHook => (), $crate::mkb::process_inputs, |cx| {
                cx.process_inputs_hook.borrow().call();
                $(
                    $crate::app_modules!(@tick $module_name {$($event),*} cx);
                )*
            });

            // Draw functions hook
            $crate::hook!(DrawDebugTextHook => (), $crate::mkb::draw_debugtext, |cx| {
                cx.draw.borrow().predraw();
                $(
                    $crate::app_modules!(@draw $module_name {$($event),*} cx);
                )*
                cx.draw_debug_text_hook.borrow().call();
            });

            // MainGameLoad calls
            $crate::hook!(OSLinkHook,
                    rel_buffer: *mut $crate::mkb::OSModuleHeader,
                    bss_buffer: *mut core::ffi::c_void => u8,
                    $crate::mkb::OSLink,
                    |rel_buffer, bss_buffer, cx| {
                let ret = cx.oslink_hook.borrow().call(rel_buffer, bss_buffer);

                let module_id = $crate::utils::relutil::ModuleId::try_from(unsafe{*rel_buffer}.info.id);
                if let Ok($crate::utils::relutil::ModuleId::MainGame) = module_id {
                }

                ret
            });
        }
    };

    // Generate calls to on_main_loop_load functions
    (@main_loop_load $module_name:ident {MainLoopLoad $(, $event:ident)* $(,)?} $cx:ident) => {
        paste::paste! {
            $cx.[<$module_name:snake>].borrow_mut().on_main_loop_load($cx);
        }
        $crate::app_modules!(@main_loop_load $module_name {$($event),*} $cx);
    };
    (@main_loop_load $module_name:ident {$other_event:ident $(, $event:ident)* $(,)?} $cx:ident) => {
        $crate::app_modules!(@main_loop_load $module_name {$($event),*} $cx);
    };
    (@main_loop_load $module_name:ident {} $cx:ident) => {};

    // Generate calls to on_main_game_load functions
    (@main_game_load $module_name:ident {MainGameLoad $(, $event:ident)* $(,)?} $cx:ident) => {
        paste::paste! {
            $cx.[<$module_name:snake>].borrow_mut().on_main_game_load($cx);
        }
        $crate::app_modules!(@main_game_load $module_name {$($event),*} $cx);
    };
    (@main_game_load $module_name:ident {$other_event:ident $(, $event:ident)* $(,)?} $cx:ident) => {
        $crate::app_modules!(@main_game_load $module_name {$($event),*} $cx);
    };
    (@main_game_load $module_name:ident {} $cx:ident) => {};

    // Generate calls to tick functions
    (@tick $module_name:ident {Tick $(, $event:ident)* $(,)?} $cx:ident) => {
        paste::paste! {
            $cx.[<$module_name:snake>].borrow_mut().tick($cx);
        }
        $crate::app_modules!(@tick $module_name {$($event),*} $cx);
    };
    (@tick $module_name:ident {$other_event:ident $(, $event:ident)* $(,)?} $cx:ident) => {
        $crate::app_modules!(@tick $module_name {$($event),*} $cx);
    };
    (@tick $module_name:ident {} $cx:ident) => {};

    // Generate calls to draw functions
    (@draw $module_name:ident {Draw $(, $event:ident)* $(,)?} $cx:ident) => {
        paste::paste! {
            $cx.[<$module_name:snake>].borrow_mut().draw($cx);
        }
        $crate::app_modules!(@draw $module_name {$($event),*} $cx);
    };
    (@draw $module_name:ident {$other_event:ident $(, $event:ident)* $(,)?} $cx:ident) => {
        $crate::app_modules!(@draw $module_name {$($event),*} $cx);
    };
    (@draw $module_name:ident {} $cx:ident) => {};
}
