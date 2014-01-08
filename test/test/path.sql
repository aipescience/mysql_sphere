USE spheretest;

SET sphere_outprec=8;

--
-- ellipse and path
--
  
-- negators , commutator @,&&

SELECT srcontainsl( spath( '{(280d, -9d),(280d, -8d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spath( '{(280d, -9d),(280d,-12d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spath( '{(280d,-11d),(280d,-12d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -9d),(280d, -8d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -9d),(280d,-12d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d,-11d),(280d,-12d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srnotcontainsl( spath( '{(280d, -9d),(280d, -8d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srnotcontainsl( spath( '{(280d, -9d),(280d,-12d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srnotcontainsl( spath( '{(280d,-11d),(280d,-12d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT snotoverlaps( spath( '{(280d, -9d),(280d, -8d)}' ),  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT snotoverlaps( spath( '{(280d, -9d),(280d,-12d)}' ),  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT snotoverlaps( spath( '{(280d,-11d),(280d,-12d)}' ),  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT slcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spath( '{(280d, -9d),(280d, -8d)}' ) );
SELECT slcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spath( '{(280d, -9d),(280d,-12d)}' ) );
SELECT slcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spath( '{(280d,-11d),(280d,-12d)}' ) );
SELECT soverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spath( '{(280d, -9d),(280d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spath( '{(280d, -9d),(280d,-12d)}' ) );
SELECT soverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spath( '{(280d,-11d),(280d,-12d)}' ) );
SELECT slnotcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' ) ,  spath( '{(280d, -9d),(280d, -8d)}' ) );
SELECT slnotcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' ) ,  spath( '{(280d, -9d),(280d,-12d)}' ) );
SELECT slnotcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' ) ,  spath( '{(280d,-11d),(280d,-12d)}' ) );
SELECT snotoverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' ) ,  spath( '{(280d, -9d),(280d, -8d)}' ) );
SELECT snotoverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' ) ,  spath( '{(280d, -9d),(280d,-12d)}' ) );
SELECT snotoverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' ) ,  spath( '{(280d,-11d),(280d,-12d)}' ) );

-- path is a line , ellipse is point
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{0d,0d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{0d,0d},(280d, -8d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{0d,0d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{0d,0d},(280d, -8d),90d>' ) );
-- path is a line , ellipse is circle
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{5d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{5d,5d},(280d, -8d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{5d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{5d,5d},(280d, -8d),90d>' ) );
-- path is a line , ellipse is path
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{5d,0d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{5d,0d},(280d, -8d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{5d,0d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{5d,0d},(280d, -8d),90d>' ) );
-- path is a line , ellipse is a real ellipse
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spath( '{(280d, -8d),(280d, -9d)}' )  ,  sellipse( '<{10d,5d},(280d, -8d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spath( '{(280d, -8d),(280d, -9d)}' ) ,  sellipse( '<{10d,5d},(280d, -8d),90d>' ) );

--
-- checking path other operators
--
  
set @poly  = ( SELECT spoly ('{(0.1,0),(0.2,0),(0.2,0.1),(0.3,0.1),(0.3,-0.1),(0.4,-0.1),(0.5,0.1),(0.4,0.2),(0.1,0.2)}' ) );
set @path1 = ( SELECT spath ('{(0.1,0),(0.2,0),(0.2,0.1),(0.3,0.1),(0.3,-0.1),(0.4,-0.1),(0.5,0.1),(0.4,0.2),(0.1,0.2)}' ) );
set @path2 = ( SELECT spath ('{(0,0),(1,0),(2,0),(3,0)}' ) );
set @path3 = ( SELECT spath ('{(0,0),(0,1),(0,1.5)}' ) );

SELECT slength( spath( '{(0,0),(1,0),(2,0),(3,0)}' ) );
SELECT sequal( @path1 ,  @path2);
SELECT sequal( @path1 ,  @path1);
SELECT snotequal( @path1 ,  @path2 );
SELECT snotequal( @path1 ,  @path1 );
SELECT soverlaps( @poly , @path1 );
SELECT soverlaps( @path1, @poly  );
SELECT srcontainsl( @path1,  @poly  );
SELECT srcontainsl( @path2,  @poly  );
SELECT soverlaps( @path1, @path1 );
SELECT soverlaps( @path1, @path1 );
SELECT soverlaps( @poly , @path2 );
SELECT soverlaps( @path2, @poly  );
SELECT soverlaps( @path2, @path1 );
SELECT soverlaps( @poly , @path3 );
SELECT soverlaps( @path3, @poly  );
SELECT soverlaps( @path3, @path1 );
SELECT soverlaps( @path3, @path2 );
SELECT srcontainsl( @path1,  scircle( '<(0,1),1>' ) );
SELECT srcontainsl( @path3,  scircle( '<(0,1),1>' ) );
SELECT srcontainsl( @path3,  scircle( '<(0,1),0.7>' ) );
SELECT soverlaps( @path1, scircle( '<(0,1),1>' ) );
SELECT soverlaps( @path3, scircle( '<(0,1),1>' ) );
SELECT soverlaps( @path3, scircle( '<(0,1),0.7>' ) );
SELECT soverlaps( @path3, scircle( '<(0,-1),0.7>' ) );
SELECT srcontainsl( @path3,  scircle( '<(0,-1),0.7>' ) );
SELECT soverlaps( @path3, sline ( spoint( '(0,-1)' ), spoint( '(0,1)' ) ) );
SELECT soverlaps( @path3, sline ( spoint( '(-1,0)' ), spoint( '(1,0)' ) ) );
SELECT soverlaps( @path3, sline ( spoint( '(-1,0)' ), spoint( '(-0.3,0)' ) ) );
SELECT srcontainsl( spath( '{(0.11,0.15),(0.12,0.15),(0.13,0.15)}' ), @poly );

-- create path
SELECT sstr( spath_aggr(data.p) ) FROM ( SELECT spoint( '(0,1)' ) as p UNION ALL SELECT spoint( '(1,1)' ) UNION ALL SELECT spoint( '(1,0)' ) ) AS data ;

SET sphere_outmode="DEG";

-- test stored data
SELECT sstr( spoint(path, 2) ) FROM spherepath;

