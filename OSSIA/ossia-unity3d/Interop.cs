using System.Collections.Generic;
using System.Runtime;
using System.Runtime.InteropServices;
using System;

namespace Ossia
{
  [StructLayout(LayoutKind.Sequential)]
  public struct vec2f
  {
    public float f1, f2;
  }
  [StructLayout(LayoutKind.Sequential)]
  public struct vec3f
  {
    public float f1, f2, f3;
  }
  [StructLayout(LayoutKind.Sequential)]
  public struct vec4f
  {
    public float f1, f2, f3, f4;
  }
  internal class Network
  {
    public delegate void ossia_value_callback(IntPtr ctx, IntPtr val);
    public delegate void ossia_node_callback(IntPtr ctx, IntPtr node);
    public delegate void ossia_parameter_callback(IntPtr ctx, IntPtr addr);

    //// Protocol ////
    [DllImport ("ossia")]
    public static extern IntPtr ossia_protocol_local_create ();

    [DllImport ("ossia")]
    public static extern void ossia_protocol_local_expose_to (
      IntPtr local_protocol,
      IntPtr remote_protocol);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_protocol_osc_create (
      string ip,
      int in_port,
      int out_port);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_protocol_minuit_create (
      string name,
      string ip,
      int in_port,
      int out_port);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_protocol_oscquery_server_create(
      int osc_port,
      int ws_port);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_protocol_oscquery_mirror_create(
      string host);

    [DllImport ("ossia")]
    public static extern void ossia_protocol_free(
         IntPtr proto);


    //// Device ////
    [DllImport ("ossia")]
    public static extern IntPtr ossia_device_create (
      IntPtr protocol,
      string name);

    [DllImport ("ossia")]
    public static extern void ossia_device_free (
      IntPtr device);

    [DllImport ("ossia")]
    public static extern int ossia_device_update_namespace (
      IntPtr device);


    [DllImport ("ossia")]
    public static extern IntPtr ossia_device_get_name (
      IntPtr device);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_device_get_root_node (
      IntPtr device);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_device_add_node_created_callback(
      IntPtr device,
      IntPtr callback,
      IntPtr ctx);
    [DllImport ("ossia")]
    public static extern void ossia_device_remove_node_created_callback(
      IntPtr device,
      IntPtr index);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_device_add_node_removing_callback(
      IntPtr device,
      IntPtr callback,
      IntPtr ctx);
    [DllImport ("ossia")]
    public static extern void ossia_device_remove_node_removing_callback(
      IntPtr device,
      IntPtr index);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_device_add_parameter_deleting_callback(
      IntPtr node,
      IntPtr callback,
      IntPtr ctx);
    [DllImport ("ossia")]
    public static extern void ossia_device_remove_parameter_deleting_callback(
      IntPtr node,
      IntPtr index);

    [DllImport ("ossia")]
    public static extern void ossia_device_reset_static();


    //// Node ////
    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_find(
      IntPtr root,
      string name);
    [DllImport ("ossia")]
    public static extern void ossia_node_find_pattern(
      IntPtr root,
      string pattern,
      out IntPtr data,
      out UIntPtr size);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_create(
      IntPtr root,
      string name);
    [DllImport ("ossia")]
    public static extern void ossia_node_create_pattern(
      IntPtr root,
      string pattern,
      out IntPtr data,
      out UIntPtr size);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_add_child (
      IntPtr node,
      string name);
    [DllImport ("ossia")]
    public static extern void ossia_node_remove_child (
      IntPtr node,
      IntPtr child);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_get_name (
      IntPtr node);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_get_device (
      IntPtr node);

    [DllImport ("ossia")]
    public static extern int ossia_node_child_size (
      IntPtr node);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_get_child (
      IntPtr node,
      int child_n);
    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_find_child (
      IntPtr node,
      string name);


    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_create_parameter(
      IntPtr node,
      ossia_type type);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_get_parameter(
      IntPtr node);

    [DllImport ("ossia")]
    public static extern void ossia_node_remove_parameter (
      IntPtr node,
      IntPtr property);


    [DllImport ("ossia")]
    public static extern IntPtr ossia_node_add_deleting_callback(
      IntPtr node,
      IntPtr callback,
      IntPtr ctx);
    [DllImport ("ossia")]
    public static extern void ossia_node_remove_deleting_callback(
      IntPtr node,
      IntPtr index);

    [DllImport("ossia")]
    public static extern void ossia_node_array_free (
      IntPtr ar);

    //// Parameter ////
    [DllImport ("ossia")]
    public static extern IntPtr ossia_parameter_get_node (
      IntPtr property);

    [DllImport ("ossia")]
    public static extern void ossia_parameter_set_access_mode (
      IntPtr property,
      ossia_access_mode am);

    [DllImport ("ossia")]
    public static extern ossia_access_mode ossia_parameter_get_access_mode (
      IntPtr property);


    [DllImport ("ossia")]
    public static extern void ossia_parameter_set_bounding_mode (
      IntPtr property,
      ossia_bounding_mode bm);

    [DllImport ("ossia")]
    public static extern ossia_bounding_mode ossia_parameter_get_bounding_mode (
      IntPtr property);


    [DllImport ("ossia")]
    public static extern void ossia_parameter_set_domain (
      IntPtr property,
      IntPtr domain);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_parameter_get_domain (
      IntPtr property);


    [DllImport ("ossia")]
    public static extern void ossia_parameter_set_value (
      IntPtr property,
      IntPtr value);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_parameter_get_value (
      IntPtr property);



    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_value (
      IntPtr property,
      IntPtr value);


    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_impulse(
      IntPtr property);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_i(
      IntPtr property,
      int value);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_b(
      IntPtr property,
      int b);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_f(
      IntPtr property,
      float value);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_2f(
      IntPtr property,
      float v1, float v2);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_3f(
      IntPtr property,
      float v1, float v2, float v3);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_4f(
      IntPtr property,
      float v1, float v2, float v3, float v4);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_c(
      IntPtr property,
      char value);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_s(
      IntPtr property,
      string value);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_in(
      IntPtr property,
      int[] value,
      int sz);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_fn(
      IntPtr property,
      float[] value,
      int sz);
    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_cn( // pushed as a string
      IntPtr property,
      char[] value,
      int sz);


    [DllImport ("ossia")]
    public static extern IntPtr ossia_parameter_fetch_value (
      IntPtr property);


    [DllImport ("ossia")]
    public static extern IntPtr ossia_parameter_add_callback (
      IntPtr property,
      IntPtr callback,
      IntPtr ctx);

    [DllImport ("ossia")]
    public static extern void ossia_parameter_push_callback(
      IntPtr property,
      IntPtr callback,
      IntPtr ctx);

    [DllImport ("ossia")]
    public static extern void ossia_parameter_remove_callback (
      IntPtr property,
      IntPtr index);

    [DllImport ("ossia")]
    public static extern void ossia_parameter_free_callback_idx(
      IntPtr index);


    //// Domain ////
    [DllImport ("ossia")]
    public static extern IntPtr ossia_domain_make_min_max (
      IntPtr min,
      IntPtr max);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_domain_get_min (
      IntPtr domain);

    [DllImport ("ossia")]
    public static extern void ossia_domain_set_min (
      IntPtr domain,
      IntPtr value);


    [DllImport ("ossia")]
    public static extern IntPtr ossia_domain_get_max (
      IntPtr domain);

    [DllImport ("ossia")]
    public static extern void ossia_domain_set_max (
      IntPtr domain,
      IntPtr value);


    [DllImport ("ossia")]
    public static extern void ossia_domain_free (
      IntPtr property);

    //// Value ////

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_impulse ();

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_int (int value);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_float (float value);
    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_2f(
      float v1, float v2);
    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_3f(
      float v1, float v2, float v3);
    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_4f(
      float v1, float v2, float v3, float v4);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_bool (int value);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_char (char value);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_string (string value);

    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_tuple (IntPtr[] values, UIntPtr size);
    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_fn (float[] values, UIntPtr size);
    [DllImport ("ossia")]
    public static extern IntPtr ossia_value_create_in (int[] values, UIntPtr size);


    [DllImport ("ossia")]
    public static extern void ossia_value_free (IntPtr value);


    [DllImport ("ossia")]
    public static extern ossia_type ossia_value_get_type (IntPtr type);

    [DllImport ("ossia")]
    public static extern int ossia_value_to_int (IntPtr val);

    [DllImport ("ossia")]
    public static extern float ossia_value_to_float (IntPtr val);

    [DllImport ("ossia")]
    public static extern vec2f ossia_value_to_2f (IntPtr val);
    [DllImport ("ossia")]
    public static extern vec3f ossia_value_to_3f (IntPtr val);
    [DllImport ("ossia")]
    public static extern vec4f ossia_value_to_4f (IntPtr val);

    [DllImport ("ossia")]
    public static extern bool ossia_value_to_bool (IntPtr val);

    [DllImport ("ossia")]
    public static extern char ossia_value_to_char (IntPtr val);

    [DllImport ("ossia")]
    public unsafe static extern void ossia_value_to_byte_array (
      IntPtr val_in,
      out IntPtr val_out,
      out UIntPtr size);

    [DllImport ("ossia")]
    public static extern void ossia_value_to_tuple (
      IntPtr val_in,
      out IntPtr val_out,
      out UIntPtr size);
    [DllImport ("ossia")]
    public static extern void ossia_value_free_tuple (IntPtr[] tpl);

    [DllImport ("ossia")]
    public static extern void ossia_value_to_fn(
      IntPtr val_in,
      out IntPtr val_out,
      out UIntPtr size);
    [DllImport ("ossia")]
    public static extern void ossia_value_free_fn (IntPtr tpl);


    [DllImport ("ossia")]
    public static extern void ossia_value_to_in(
      IntPtr val_in,
      out IntPtr val_out,
      out UIntPtr size);
    [DllImport ("ossia")]
    public static extern void ossia_value_free_in (IntPtr tpl);


    [DllImport ("ossia")]
    public static extern int ossia_value_convert_int (IntPtr val);

    [DllImport ("ossia")]
    public static extern float ossia_value_convert_float (IntPtr val);

    [DllImport ("ossia")]
    public static extern int ossia_value_convert_bool (IntPtr val);

    [DllImport ("ossia")]
    public static extern char ossia_value_convert_char (IntPtr val);

    [DllImport ("ossia")]
    public static extern string ossia_value_convert_string (
      IntPtr val_in,
      IntPtr val_out,
      IntPtr size);

    [DllImport ("ossia")]
    public static extern void ossia_value_convert_tuple (
      IntPtr val_in,
      out IntPtr val_out,
      out UIntPtr size);



    [DllImport ("ossia")]
    public static extern void ossia_string_free( IntPtr str );

    [DllImport ("ossia")]
    public static extern void ossia_set_debug_logger( IntPtr fp );
  }
}

