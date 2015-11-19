{
        if ( ++r == 1 )
                timestamp_min = $2
        if ( NF > max_NF )
                max_NF = NF
        header[r] = $1
        timestamp[r] = $2 - timestamp_min
        for (i=3; i <= NF; ++i) {
                x_path[r][i-2] = $i
                if ( $i > max_x[i-2] )
                        max_x[i-2] = $i
        }
}

END {
        records = r
        N = max_NF - 2
        base[N] = 1
        for (i=N-1; i >= 1; --i)
                base[i] = (max_x[i+1] + 1) * base[i+1]
        for (r=1; r <= records; ++r) {
                for (i=1; i <= N; ++i)
                        x[r] += x_path[r][i] * base[i]
                index_of_x[x[r]] = r
        }
        x_ordered_length = asorti(index_of_x, x_ordered, "@ind_num_asc")
        for (r=1; r <= x_ordered_length; ++r)
                x_rank[x_ordered[r]] = r
        for (r=1; r <= records; ++r)
                x[r] = x_rank[x[r]]
        for (r=1; r <= records; r+=2) {
                if ( header[r] ~ /^Solve/ )
                        print_vector()
        }
        printf("\n\n");
        for (r = 1; r <= records; r += 2) {
                if ( header[r] ~ /^Simulate/ )
                        print_vector()
        }
}

function print_vector () {
        print_edge(r)
        printf("\t")
        print_edge(r + 1)
        printf("\n")
}

function print_edge (record) {
        printf("%d\t", timestamp[record])
        printf("%d\t", x[record])
}
