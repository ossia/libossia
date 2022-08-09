#pragma once
#include <ossia/dataflow/token_request.hpp>

#include <ossia-qt/time.hpp>

#include <QDataStream>

#include <verdigris>

inline QDataStream& operator<<(QDataStream& i, const ossia::token_request& sel)
{
  return i << sel.prev_date << sel.date << sel.parent_duration << sel.offset << sel.speed
           << sel.tempo << sel.signature << sel.musical_start_last_signature
           << sel.musical_start_last_bar << sel.musical_start_position
           << sel.musical_end_last_bar << sel.musical_end_position
           << sel.start_discontinuous << sel.end_discontinuous;
}

inline QDataStream& operator>>(QDataStream& i, ossia::token_request& sel)
{
  return i >> sel.prev_date >> sel.date >> sel.parent_duration >> sel.offset >> sel.speed
         >> sel.tempo >> sel.signature >> sel.musical_start_last_signature
         >> sel.musical_start_last_bar >> sel.musical_start_position
         >> sel.musical_end_last_bar >> sel.musical_end_position
         >> sel.start_discontinuous >> sel.end_discontinuous;
}

Q_DECLARE_METATYPE(ossia::token_request)
W_REGISTER_ARGTYPE(ossia::token_request)
