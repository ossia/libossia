set(API_HEADERS
    ## Editor ##
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/prefix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/context.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/apply.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/algorithms.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/callback_container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/destination_index.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/json.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/json_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/any_map.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/string_map.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/math.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/logger.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/optional.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/locked_container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/ptr_container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/safe_vec.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/mutex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/automation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/tinyspline.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/tinysplinecpp.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/spline.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/gradient.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/curve_value_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_abstract.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/behavior.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/easing.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/empty.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/linear.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/power.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/sin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/behavior_variant_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/exceptions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/editor.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/loop/loop.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/mapper/mapper.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_atom.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_composition.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_generic.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_not.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_bool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_pulse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/operators.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/scenario.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_constraint.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_event.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_value.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/clock.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/custom_state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/destination_qualifiers.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/detail/state_execution_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/detail/state_flatten_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/detail/state_print_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/message.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state_element.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/functional_state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state_element_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/destination.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/impulse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_conversion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/detail/value_conversion_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_algorithms.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_variant_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/vec.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/detail/dataspace_parse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/detail/dataspace_merge.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/detail/dataspace_convert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/detail/make_value.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/detail/make_unit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_strong_variants.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_variant_visitors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_visitors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_parse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_base_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_base_defs_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_base_variants.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/value_with_unit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/position.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/orientation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/angle.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/speed.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/distance.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/color.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/gain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/temperature.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/time.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/parameter_properties.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/network_logger.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/websocket_log_sink.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/exceptions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/network.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/misc_visitors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/ossia.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/editor.hpp"

    ## Network ##
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_base_variant.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_functions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_conversion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/numeric_domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/min_max.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/clamp_visitors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/generic_domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/array_domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/value_set_domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/apply_domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_base_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_variant_impl.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/parameter_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/node_functions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/listening.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/node_attributes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/value_callback.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/name_validation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/debug.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/extended_types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/complex_type.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/generic_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/generic_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/generic_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/alias_node.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/local/local.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/minuit/minuit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/osc.hpp"


    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/zeroconf/zeroconf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/minuit/detail/minuit_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/minuit/detail/minuit_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/minuit/detail/minuit_name_table.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/detail/message_generator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/detail/receiver.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/detail/sender.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/detail/osc.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/detail/osc_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/detail/string_view.hpp"
    )

set(SRCS
    ${API_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/ossia.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/automation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/tinyspline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/tinysplinecpp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/spline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/gradient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_atom.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_composition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_generic.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_not.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_bool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_pulse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/loop/loop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/mapper/mapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/scenario_execution.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/scenario_offset.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/scenario.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_constraint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_event.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_process.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/clock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state_element.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/dataspace_visitors.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/dataspace/detail/dataspace_impl.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/domain_base.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/detail/domain_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/clamp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/clamp_min.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/clamp_max.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/wrap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/domain/fold.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/parameter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/name_validation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/node_functions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/node_attributes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/base/protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/extended_types.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/path.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/complex_type.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/common/debug.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/generic_parameter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/generic_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/generic_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/generic/alias_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/local/local.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/minuit/minuit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/minuit/detail/minuit_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/zeroconf/zeroconf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/osc/osc.cpp"
)

set(OSSIA_C_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia-c.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/log/ossia_log.h"

  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/preset/preset.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/preset/result.h"
    )

set(OSSIA_C_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_utils.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_protocol.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_node.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_device.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_parameter.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_value.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/ossia/ossia_domain.cpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/log/ossia_log.cpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/preset/preset.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/preset/preset.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/preset/exception.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c/preset/exception.cpp"
    )

set(OSSIA_CPP_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-cpp/ossia-cpp98.hpp"
    )

set(OSSIA_CPP_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-cpp/ossia-cpp98.cpp"
    )

set(OSSIA_MIDI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/detail/channel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/detail/midi_impl.hpp"
    )

set(OSSIA_MIDI_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/midi/midi_parameter.cpp")

set(OSSIA_OSCQUERY_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/oscquery_server.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/oscquery_client.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/oscquery_mirror.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/outbound_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/outbound_visitor_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/server.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/client.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/http_client.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/attributes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/typetag.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/http_query_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_query_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/get_query_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/query_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_writer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_reader_detail.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_writer_detail.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/value_to_json.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/domain_to_json.hpp"
    )

set(OSSIA_OSCQUERY_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/oscquery_server.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/oscquery_mirror.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_reader_detail.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/json_writer_detail.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/query_parser.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/attributes.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/oscquery/detail/typetag.cpp"
    )

set(OSSIA_HTTP_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_parameter_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http.hpp")

set(OSSIA_HTTP_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/http/http_parameter.cpp")

set(OSSIA_SERIAL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_parameter_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial.hpp")

set(OSSIA_SERIAL_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/serial/serial_parameter.cpp"
)


set(OSSIA_PHIDGETS_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/detail/phidgetspp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/detail/sensors.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_parameter_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets.hpp")

set(OSSIA_PHIDGETS_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/detail/phidgetspp.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/phidgets/phidgets_parameter.cpp")


set(OSSIA_WS_CLIENT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_parameter_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client.hpp")

set(OSSIA_WS_CLIENT_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/websocket-generic-client/ws_generic_client_parameter.cpp")

set(OSSIA_JAVA_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-java/ossia-java.hpp"
    )

set(OSSIA_JAVA_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-java/ossia-java.cpp"
    )
set(OSSIA_QT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_property.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_signal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_property_reader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_model_property.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_node_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qt_object_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qt_property_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qt_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/js_utilities.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/name_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_context.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_plugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/value_metatypes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/metatypes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_logger.hpp"
    )
set(OSSIA_QT_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_property.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_parameter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_signal.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_property_reader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_model_property.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_node_base.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qml_logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qt_object_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qt_property_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/device/qt_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/js_utilities.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_plugin.cpp"
    )
set(OSSIA_QML_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Binding.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Callback.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/MidiSink.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/MidiSource.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Node.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSC.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSCQueryClient.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/OSCQueryServer.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/qmldir"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Reader.qml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/Ossia/Writer.qml"
  )

set(OSSIA_DATAFLOW_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/audio_parameter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/dataflow.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/connection.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/dataflow_fwd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/execution_state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/graph_edge.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/graph_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/node_process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/port.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/graph.hpp"
)

set(OSSIA_DATAFLOW_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/audio_parameter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/graph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/port.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/graph_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/dataflow/execution_state.cpp"
)
