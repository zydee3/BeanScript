//
//#include "keyboard.h"
//
//static int keyboard_detect_id() {
//    InterceptionDevice current_keyboard = INTERCEPTION_KEYBOARD(1);
//
//    while (current_keyboard <= INTERCEPTION_KEYBOARD(INTERCEPTION_MAX_KEYBOARD)) {
//        if (interception_is_keyboard(current_keyboard)) {
//            return current_keyboard;
//        }
//
//        current_keyboard++;
//    }
//
//    assert(false, "Could not detect any keyboards.");
//}
//
//Keyboard* keyboard_new() {
//    Keyboard* keyboard = (Keyboard*) malloc(sizeof(Keyboard));
//    assert(keyboard == NULL, "Could not allocate memory for keyboard.");
//
//    keyboard->id = keyboard_detect_id();
//    keyboard->stroke = (InterceptionKeyStroke) { 0 };
//
//    keyboard->context = interception_create_context();
//    interception_set_filter(keyboard->context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_UP);
//
//    return keyboard;
//}
//
//void keyboard_delete(Keyboard** ptr_keyboard) {
//    assert(ptr_keyboard != NULL, "Attempting to delete keyboard behind NULL pointer.");
//    assert(*ptr_keyboard != NULL, "Attempting to delete NULL keyboard.");
//
//    Keyboard* keyboard = *ptr_keyboard;
//    free(keyboard);
//    *ptr_keyboard = NULL;
//}
//
//bool keyboard_send(Keyboard* keyboard, const char* key_id) {
//    assert(keyboard != NULL, "Attempting to send key to NULL keyboard.");
//    assert(key_id != NULL, "Attempting to send NULL key to keyboard.");
//
//    Key* key = key_map_get(key_id);
//    assert(key != NULL, "Attempting to send invalid key to keyboard.");
//
//    InterceptionContext context;
//    InterceptionDevice device;
//
//    InterceptionKeyStroke stroke;
//
//    context = interception_create_context();
//
//    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_UP);
//
//    while (interception_receive(context, device = interception_wait(context), (InterceptionStroke *)&stroke, 1) > 0) {
//
//        // This code simulates pressing the 'A' key once
//        stroke.code = key->code;
//        stroke.state = INTERCEPTION_KEY_DOWN;
//        interception_send(context, device, (InterceptionStroke *)&stroke, 1);
//
//        stroke.state = INTERCEPTION_KEY_UP;
//        interception_send(context, device, (InterceptionStroke *)&stroke, 1);
//
//        stroke.state = INTERCEPTION_KEY_DOWN;
//        interception_send(context, device, (InterceptionStroke *)&stroke, 1);
//
//        stroke.state = INTERCEPTION_KEY_UP;
//        interception_send(context, device, (InterceptionStroke *)&stroke, 1);
//
//        // Be careful with this infinite loop and the A key simulation.
//        // It will simulate pressing A key continuously.
//        // You might want to include a condition to break the loop.
//        break;
//    }
//
//    interception_destroy_context(context);
//
//    return true;
//}