// To use :
// download http://repo1.maven.org/maven2/net/java/dev/jna/jna/4.5.1/jna-4.5.1.jar
// put it in a "code" folder in the sketchbook
// and rename it to jna.jar (maybe not necessary ?)
// e.g. ~/sketchbook/sketch_123456/code/jna.jar

// Then change the path to libossia.so/ossia.dll/libossia.dylib
package io.ossia;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;

public interface Ossia extends Library
{
  Ossia INSTANCE = (Ossia) Native.loadLibrary("ossia", Ossia.class);

    //// Protocol ////
     Pointer ossia_protocol_multiplex_create ();

     void ossia_protocol_multiplex_expose_to (
      Pointer local_protocol,
      Pointer remote_protocol);

     Pointer ossia_protocol_osc_create (
      String ip,
      int in_port,
      int out_port);

     Pointer ossia_protocol_minuit_create (
      String name,
      String ip,
      int in_port,
      int out_port);

     Pointer ossia_protocol_oscquery_server_create(
      int osc_port,
      int ws_port);

     Pointer ossia_protocol_oscquery_mirror_create(
      String host);

     void ossia_protocol_free(
         Pointer proto);


    //// Device ////
     Pointer ossia_device_create (
      Pointer protocol,
      String name);

     void ossia_device_free (
      Pointer device);

     int ossia_device_update_namespace (
      Pointer device);


     Pointer ossia_device_get_name (
      Pointer device);

     Pointer ossia_device_get_root_node (
      Pointer device);

     Pointer ossia_device_add_node_created_callback(
      Pointer device,
      Pointer callback,
      Pointer ctx);
     void ossia_device_remove_node_created_callback(
      Pointer device,
      Pointer index);

     Pointer ossia_device_add_node_removing_callback(
      Pointer device,
      Pointer callback,
      Pointer ctx);
     void ossia_device_remove_node_removing_callback(
      Pointer device,
      Pointer index);

     Pointer ossia_device_add_parameter_deleting_callback(
      Pointer node,
      Pointer callback,
      Pointer ctx);
     void ossia_device_remove_parameter_deleting_callback(
      Pointer node,
      Pointer index);

     void ossia_device_reset_static();


    //// Node ////
     Pointer ossia_node_find(
      Pointer root,
      String name);
     void ossia_node_find_pattern(
      Pointer root,
      String pattern,
      Pointer data,
      Pointer size);

     Pointer ossia_node_create(
      Pointer root,
      String name);
     void ossia_node_create_pattern(
      Pointer root,
      String pattern,
      Pointer data,
      Pointer size);

     Pointer ossia_node_add_child (
      Pointer node,
      String name);
     void ossia_node_remove_child (
      Pointer node,
      Pointer child);

     Pointer ossia_node_get_name (
      Pointer node);

     Pointer ossia_node_get_device (
      Pointer node);

     int ossia_node_child_size (
      Pointer node);

     Pointer ossia_node_get_child (
      Pointer node,
      int child_n);
     Pointer ossia_node_find_child (
      Pointer node,
      String name);


     Pointer ossia_node_create_parameter(
      Pointer node,
      int type);

     Pointer ossia_node_get_parameter(
      Pointer node);

     void ossia_node_remove_parameter (
      Pointer node,
      Pointer property);


     Pointer ossia_node_add_deleting_callback(
      Pointer node,
      Pointer callback,
      Pointer ctx);
     void ossia_node_remove_deleting_callback(
      Pointer node,
      Pointer index);

     void ossia_node_array_free (
      Pointer ar);


     void ossia_node_set_description(
        Pointer node,
        String description);
     String ossia_node_get_description(
        Pointer node);
     void ossia_node_set_extended_type(
        Pointer node,
        String description);
     String ossia_node_get_extended_type(
        Pointer node);


     void ossia_node_set_tags(
        Pointer node,
        Pointer tags,
        SizeT sz);
     void ossia_node_get_tags(
        Pointer node,
        Pointer tags,
        Pointer sz);
     void ossia_tags_free(
        Pointer tags,
        SizeT sz);

     void ossia_node_set_hidden(
        Pointer node,
        int hidden);
     int ossia_node_get_hidden(
        Pointer node);

     void ossia_node_set_refresh_rate(
        Pointer node,
        int refresh_rate);
     void ossia_node_unset_refresh_rate(
        Pointer node);
     int ossia_node_get_refresh_rate(
        Pointer node,
        Pointer ok);

     void ossia_node_set_priority(
        Pointer node,
        float priority);
     void ossia_node_unset_priority(
        Pointer node);
     float ossia_node_get_priority(
        Pointer node,
        Pointer ok);

     void ossia_node_set_value_step_size(
        Pointer node,
        double value_step_size);
     void ossia_node_unset_value_step_size(
        Pointer node);
     double ossia_node_get_value_step_size(
        Pointer node,
        Pointer ok);

     void ossia_node_set_instance_bounds(
        Pointer node,
        int min, int max);
     void ossia_node_unset_instance_bounds(
        Pointer node);
     void ossia_node_get_instance_bounds(
        Pointer node,
        Pointer min, Pointer max,
        Pointer ok);

     void ossia_node_set_default_value(
        Pointer node,
        Pointer default_value);
     Pointer ossia_node_get_default_value(
        Pointer node);


    //// Parameter ////
     Pointer ossia_parameter_get_node (
      Pointer property);

     void ossia_parameter_set_access_mode (
      Pointer property,
      int am);

     int ossia_parameter_get_access_mode (
      Pointer property);


     void ossia_parameter_set_unit(
      Pointer address,
      String unit);
     String ossia_parameter_get_unit(
      Pointer address);

     void ossia_parameter_set_bounding_mode (
      Pointer property,
      int bm);

     int ossia_parameter_get_bounding_mode (
      Pointer property);


     void ossia_parameter_set_domain (
      Pointer property,
      Pointer domain);

     Pointer ossia_parameter_get_domain (
      Pointer property);


     void ossia_parameter_set_value (
      Pointer property,
      Pointer value);

     Pointer ossia_parameter_get_value (
      Pointer property);



     void ossia_parameter_push_value (
      Pointer property,
      Pointer value);


     void ossia_parameter_push_impulse(
      Pointer property);
     void ossia_parameter_push_i(
      Pointer property,
      int value);
     void ossia_parameter_push_b(
      Pointer property,
      int b);
     void ossia_parameter_push_f(
      Pointer property,
      float value);
     void ossia_parameter_push_2f(
      Pointer property,
      float v1, float v2);
     void ossia_parameter_push_3f(
      Pointer property,
      float v1, float v2, float v3);
     void ossia_parameter_push_4f(
      Pointer property,
      float v1, float v2, float v3, float v4);
     void ossia_parameter_push_c(
      Pointer property,
      byte value);
     void ossia_parameter_push_s(
      Pointer property,
      String value);
     void ossia_parameter_push_in(
      Pointer property,
      Pointer value,
      SizeT sz);
     void ossia_parameter_push_fn(
      Pointer property,
      Pointer value,
      SizeT sz);
     void ossia_parameter_push_cn( // pushed as a string
      Pointer property,
      Pointer value,
      SizeT sz);


     Pointer ossia_parameter_fetch_value (
      Pointer property);

     void ossia_parameter_set_listening (
      Pointer address,
      int listening);

     Pointer ossia_parameter_add_callback (
      Pointer property,
      Pointer callback,
      Pointer ctx);

     void ossia_parameter_push_callback(
      Pointer property,
      Pointer callback,
      Pointer ctx);

     void ossia_parameter_remove_callback (
      Pointer property,
      Pointer index);

     void ossia_parameter_free_callback_idx(
      Pointer index);


    //// Domain ////
     Pointer ossia_domain_make_min_max (
      Pointer min,
      Pointer max);

     Pointer ossia_domain_get_min (
      Pointer domain);

     void ossia_domain_set_min (
      Pointer domain,
      Pointer value);


     Pointer ossia_domain_get_max (
      Pointer domain);

     void ossia_domain_set_max (
      Pointer domain,
      Pointer value);

     void ossia_domain_get_values(
      Pointer domain,
      Pointer values,
      Pointer sz);

     void ossia_domain_set_values(
      Pointer domain,
      Pointer value,
      SizeT n);

     void ossia_domain_free (
      Pointer d);

    //// Value ////

     Pointer ossia_value_create_impulse ();

     Pointer ossia_value_create_int (int value);

     Pointer ossia_value_create_float (float value);
     Pointer ossia_value_create_2f(
      float v1, float v2);
     Pointer ossia_value_create_3f(
      float v1, float v2, float v3);
     Pointer ossia_value_create_4f(
      float v1, float v2, float v3, float v4);

     Pointer ossia_value_create_bool (int value);

     Pointer ossia_value_create_char (byte value);

     Pointer ossia_value_create_string (String value);
     Pointer ossia_value_create_byte_array (Pointer value, SizeT sz);

     Pointer ossia_value_create_list (Pointer values, Pointer size);
     Pointer ossia_value_create_fn (Pointer values, Pointer size);
     Pointer ossia_value_create_in (Pointer values, Pointer size);


     void ossia_value_free (Pointer value);


     int ossia_value_get_type (Pointer type);
     int ossia_value_to_int (Pointer val);
     float ossia_value_to_float (Pointer val);
     Vec2F.ByValue ossia_value_to_2f (Pointer val);
     Vec3F.ByValue ossia_value_to_3f (Pointer val);
     Vec4F.ByValue ossia_value_to_4f (Pointer val);
     boolean ossia_value_to_bool (Pointer val);
     byte ossia_value_to_char (Pointer val);

    void ossia_value_to_byte_array (
      Pointer val_in,
      Pointer val_out,
      Pointer size);

     void ossia_value_to_list (
      Pointer val_in,
      Pointer val_out,
      Pointer size);
     void ossia_value_free_list (Pointer[] tpl);

     void ossia_value_to_fn(
      Pointer val_in,
      Pointer val_out,
      Pointer size);
     void ossia_value_free_fn (Pointer tpl);

     void ossia_value_to_in(
      Pointer val_in,
      Pointer val_out,
      Pointer size);
     void ossia_value_free_in (Pointer tpl);


     int ossia_value_convert_int (Pointer val);
     float ossia_value_convert_float (Pointer val);
     boolean ossia_value_convert_bool (Pointer val);
     byte ossia_value_convert_char (Pointer val);

     String ossia_value_convert_byte_array (
      Pointer val_in,
      Pointer val_out,
      Pointer size);

     void ossia_value_convert_list (
      Pointer val_in,
      Pointer val_out,
      Pointer size);

      Vec2F.ByValue ossia_value_convert_2f (Pointer val);
      Vec3F.ByValue ossia_value_convert_3f (Pointer val);
      Vec4F.ByValue ossia_value_convert_4f (Pointer val);

     void ossia_string_free( Pointer str );

    /// LOG ///
     void ossia_set_debug_logger( Pointer fp );
     Pointer ossia_logger_create(String host, String app);
     void ossia_logger_init_heartbeat(Pointer log, int pid, String cmdline);
     void ossia_logger_set_level(Pointer log, int lvl);
     void ossia_log(Pointer log, int lvl, String message);
     void ossia_logger_free(Pointer log);

    /// MESSAGE QUEUE ///
    Pointer ossia_mq_create(Pointer device);
     void ossia_mq_register(Pointer mq, Pointer param);
     void ossia_mq_unregister(Pointer mq, Pointer param);
     int ossia_mq_pop(Pointer mq, PointerByReference address, PointerByReference value);
     void ossia_mq_free(Pointer mq);

}
