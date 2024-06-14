# Tests if a string has a character.
proc str_has_char {str chr} {
  set sl [string length $str]
  for {set i 0} {$i < $sl} {incr i} {
    if {[string index $str $i] == $chr} {
      return 1
    }
  }
  return 0
}

# Splits a string, taking elements that are separated by delimiters, which can
# be consecutive.
proc split_str {str_in delims} {
  set res {}
  set slen [string length $str_in]
  set step 1

  for {set i 0} {$i < $slen} {set i [expr $i + $step]} {
    set j 0
    set cch [string index $str_in $i]

    if {[str_has_char $delims $cch]} {
      # Counts delimiters and skips them
      while {[
        expr ( \
          (($i + $j) < $slen) && \
          [str_has_char $delims [string index $str_in [expr $i + $j]]] \
        )
      ]} {
        incr j
      }
    } else {
      # Counts non-delimiters and creates the splitted string
      set aux ""
      while {[
        expr ( \
          (($i + $j) < $slen) && \
          ![str_has_char $delims [string index $str_in [expr $i + $j]]] \
        )
      ]} {
        append aux [string index $str_in [expr $i + $j]]
        incr j
      }

      # Adds the splitted string
      lappend res $aux
    }

    # Jumps to the next set of delimiter or non-delimiter characters
    set step $j
  }

  return $res
}

set str_inputs {
  "."
  ".;,,.;;::;.,;.,.,:;.,;.:;"
  "abc,de.f:gh;ijkl"
  ";abc,de.f:gh;ijkl..."
  ",:,:abc;:.,.;.de.;:;f,:;:gh;:;:;:;;..,,.:;:;,:ijkl.;:,.,.,.;:;:.,,"
  "x"
  ";x"
  ";.,.x"
  ";x;"
  ",.;.,.,.,x,.;.,.,.,.::;;:,"
  "x;"
  "x;;..,:"
  "x::;;:;:;;::,.;:.,.,.,,.,:,:;:;:;y;z"
}

set delims ".,:;"

foreach str_in $str_inputs {
  puts "Input: \"${str_in}\""

  set sil [string length $str_in]
  set spr [split_str $str_in $delims]
  set sln [llength $spr]

  puts "Results ($sln):"
  if {$sln == 0} {
    puts "  (none)"
  } else {
    foreach spl_item $spr {
      puts "- $spl_item"
    }
  }
}
