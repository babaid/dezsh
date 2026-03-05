export PATH=$(echo $PATH | tr ':' '\n' | while read p; do
    [ -d "$p" ] && echo -n "$p:"
done | sed 's/:$//')
