#!/bin/bash 


GROUP=""
while read line; do {
  NEWGROUP2=$(echo "$line" | sed -n -e 's/^ *[^/].*setGroup.*\"\([^\"]*\)\".*/\1/p' )
  ENTRY=$(echo "$line" | sed -n -e 's/^ *[^/].*writeEntry.*\"\([^\"]*\)\".*/\1/p' )

  if [[ -n "${NEWGROUP2}" ]]; then
    NEWGROUP="${NEWGROUP2}"
  elif [[ -n "$ENTRY" ]] ; then 
    if [[ -n "$NEWGROUP" ]] ; then
      if [[ -n "$GROUP" ]]; then
        echo "  </group>"
      fi
      GROUP="$NEWGROUP"
      NEWGROUP=""
      echo "  <group name=\"$GROUP\">"
    fi
     
    PREFERENCESENTRY=$(echo "$line" | sed -n -e 's/^.*preferences.\(.*\)().*$/\1/p' )
    if [[ -z "$PREFERENCESENTRY" ]] ; then 
      echo "<!--  Could not understand:  \"$line\" -->"
    else
      echo "    <!-- preferencesentry = $PREFERENCESENTRY -->"
    
      TYPE=$( grep -i "$PREFERENCESENTRY *(" preferences.h | sed -n -e "s/ *\(const\)\? *\([^&]*\)\(&\)\?  *${PREFERENCESENTRY}.*/\2/p" )
      echo "    <!-- type = $TYPE  -->"
      #See http://www.kde.org/standards/kcfg/1.0/kcfg.dtd
    
      if [[ "$TYPE" == "int" ]] ; then TYPE="Int";
      elif [[ "$TYPE" == "unsigned int" ]] ; then TYPE="UInt";
      elif [[ "$TYPE" == "unsigned long" ]] ; then TYPE="UInt64";
      elif [[ "$TYPE" == "long" ]] ; then TYPE="Int64";
      elif [[ "$TYPE" == "QString" ]] ; then TYPE="String";
      elif [[ "$TYPE" == "bool" ]] ; then TYPE="Bool";
      elif [[ "$TYPE" == "QValueList<int>" ]] ; then TYPE="IntList";
      elif [[ "$TYPE" == "QValueList<bool>" ]] ; then TYPE="BoolList";
      elif [[ "$TYPE" == "QValueList<QString>" ]] ; then TYPE="StringList";
      elif [[ "$TYPE" == "QFont" ]] ; then TYPE="Font";
      elif [[ "$TYPE" == "QSize" ]] ; then TYPE="Size";
      elif [[ "$TYPE" == "QRect" ]] ; then TYPE="Rect";
      elif [[ "$TYPE" == "QColor" ]] ; then TYPE="Color";
      elif [[ "$TYPE" == "QPoint" ]] ; then TYPE="Point";
      fi
    
      echo "$PREFERENCESENTRY" | grep .toString && TYPE="String"
    
      DEFAULT=$( grep -i "[^:]set$ENTRY *(" preferences.cpp | sed -n -e 's/.*set[^(]*[(]\(.*\)[)][^)]*/\1/p' )
      
      DEFAULT2=$(echo "$DEFAULT" | sed -n -e "s/^\(\"\(.*\)\"\|\([0-9][0-9]*\|true\|false\)\)$/\2\3/p" )
      
      if [[ -z "$TYPE" ]]; then
        ISSTRING=$(echo "$DEFAULT" | sed -n -e "s/^\"\(.*\)\"$/\1/p" )
	if [[ -n "$ISSTRING" ]]; then
	  TYPE="String"
	fi
	ISNUM=$(echo "$DEFAULT" | sed -n -e "s/^\([0-9][0-9]*\)$/\1/p" )
	if [[ -n "$ISNUM" ]] ; then
	  TYPE="Int"
	fi
	ISBOOL=$(echo "$DEFAULT" | sed -n -e "s/^\(true|false\)$/\1/p" )
	if [[ -n "$ISBOOL" ]] ; then
	  TYPE="Bool"
	fi

        echo "$PREFERENCESENTRY" | grep Color && TYPE="Color"
      fi
      
      
      echo "    <entry key=\"$ENTRY\" type=\"$TYPE\">"
      if [[ -n "${DEFAULT2}" ]] ; then 
        echo "      <default>${DEFAULT2}</default>"
      else 
        echo "      <default code=\"true\">${DEFAULT}</default>"
      fi
      echo "      <label></label>"
      echo "      <whatsthis></whatsthis>"
      echo "    </entry>"
    fi
     
  fi
}
done

if [[ -n "$GROUP" ]]; then
  echo "  </group>"
fi


