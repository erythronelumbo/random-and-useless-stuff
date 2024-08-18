set DAY_NAMES {Saturday Sunday Monday Tuesday Wednesday Thursday Friday}
set ABBR_DAY_NAMES {Sa Su Mo Tu We Th Fr}
set MONTH_NAMES {
  January February March April May June July August September October November
  December
}


# Calculates the day of the week in the Gregorian calendar, based on Zeller's
# congruence.
proc day_of_week {year month day} {
  set year_ [expr $year - ($month <= 2)]
  set century [expr $year_ / 100]
  set year_of_cent [expr $year_ % 100]
  # Notes:
  # - Each i-th nibble of the `0x5305264163410` constant contains the value for
  #   `((13*(4 + ($i + 9)%12))/5)%7`, except for `i == 0`, which is ignored.
  return [
    expr \
      ( \
        $day + \
        ((0x530526416341f >> ($month << 2)) & 0xf) + \
        $year_of_cent + ($year_of_cent >> 2) + \
        ($century >> 2) + (($century << 2) + $century) \
      ) % 7
  ]
}


proc days_in_year {year fst_month num_months start_day} {
  if {[
    expr \
      ($fst_month < 1) | ($fst_month > 12) | \
      ($num_months < 1) | ($num_months > 12)
  ]} {
    puts "Failed!"
    return
  }

  global MONTH_NAMES
  global DAY_NAMES
  global ABBR_DAY_NAMES

  set is_leap [
    expr (($year & 3) == 0) & (($year % 100 != 0) | ($year % 400 == 0))
  ]

  # Acts like a lookup table for the days of each month, but 28 is subtracted.
  # 1 is added to the days in february if the year is leap.
  set days_per_month [expr 0x323233232303 | ($is_leap << 4)]

  puts "First day on calendar: [lindex $DAY_NAMES $start_day]\n"

  puts [format "====\[ Year: %4u \]====" $year]

  for {set mm_aux 0} {$mm_aux < $num_months} {incr mm_aux} {
    set mm [expr $mm_aux + $fst_month - 1]; # January is zero
    set days_in_month [expr 28 + (($days_per_month >> ($mm << 2)) & 3)]
    set month_j1 [expr $mm + 1]; # January is one

    # First day of the month
    set first_day [day_of_week $year $month_j1 1]

    set month_name [lindex $MONTH_NAMES $mm]

    set sl [string length $month_name]
    set padl [expr (20 - $sl) / 2]
    set padr [expr 20 - $padl - $sl]
    puts "[string repeat {~} $padl] $month_name [string repeat {~} $padr]"

    # Prints the names of the days of the week
    for {set i 0} {$i < 7} {incr i} {
      puts -nonewline " [lindex $ABBR_DAY_NAMES [expr ($i + $start_day) % 7]]"
    }
    puts ""

    # First non-day cells
    set first_empty_cells [expr (7 + $first_day - $start_day) % 7]

    # Number of cells occupied in the calendar
    set num_day_cells [expr $days_in_month + $first_empty_cells]
    set num_total_cells [expr (($num_day_cells + 6)/7)*7]

    for {set cr 0} {$cr < $num_total_cells} {set cr [expr $cr + 7]} {
      for {set wd 0} {$wd < 7} {incr wd} {
        set dd [expr $cr + $wd]
        if {$dd >= $first_empty_cells && $dd < $num_day_cells} {
          puts -nonewline [format " %2u" [expr $dd - $first_empty_cells + 1]]
        } else {
          puts -nonewline " --"
        }
      }
      puts ""
    }
  }
}
