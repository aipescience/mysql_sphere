SET sphere_outprec=8;

--  ellipse tests

-- equal operator
SELECT sequal(sellipse('<{0d,0d},(170d,-12d),0d>'), sellipse('<{0d,0d},(170d,-12d),0d>'));
SELECT sequal(sellipse('<{0d,0d},(170d,-12d),0d>'), sellipse('<{0d,0d},(170d,-12d),-2d>'));
SELECT sequal(sellipse('<{5d,5d},(170d,-12d),0d>'), sellipse('<{5d,5d},(170d,-12d),-2d>'));
SELECT sequal(sellipse('<{5d,2d},(170d,-12d),0d>'), sellipse('<{5d,2d},(170d,-12d),-2d>'));

--  not equal
SELECT snotequal( sellipse('<{5d,2d},(170d,-12d),0d>'), sellipse('<{5d,2d},(170d,-12d),-2d>') );
SELECT snotequal( sellipse('<{5d,2d},(170d,-12d),0d>'), sellipse('<{5d,2d},(170d,-12d), 0d>') );
SELECT sequal(sellipse('<{10d, 5d},(300d,0d),  0d>'), sellipse('<{10d,5d},(300d,0d), 180d>'));
SELECT sequal(sellipse('<{10d, 5d},(300d,0d), 90d>'), sellipse('<{10d,5d},(300d,0d), 270d>'));

--  ellipse and point
SELECT srcontainsl( spoint('(280d,-20d)')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( spoint('(280d,-10d)')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( spoint('(280d,-9.9d)') ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>'), spoint('(280d,-10d)') );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>'), spoint('(280d, -9d)') );
SELECT srnotcontainsl( spoint('(280d,-10d)')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srnotcontainsl( spoint('(280d,-9.9d)') ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>'), spoint('(280d,-10d)') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>'), spoint('(280d, -9d)') );
SELECT srcontainsl( spoint('(0d, 90d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(0d, -90d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(0d, 0d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(90d, 0d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(180d, 0d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(270d, 0d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(9d, 0d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(0d, 9d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );
SELECT srcontainsl( spoint('(0d, 4d)'), sellipse('<{10d,5d},(0d,0d),0d>')  );

-- 
-- ellipse and circle (@,&&)
-- 
--  negators and commutators

SELECT srcontainsl( scircle( '<(280d,-10d),0d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d, -9d),0d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srnotcontainsl( scircle( '<(280d,-10d),0d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srnotcontainsl( scircle( '<(280d, -9d),0d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>') , scircle( '<(280d,-10d),0d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>') , scircle( '<(280d, -9d),0d>') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>'), scircle( '<(280d,-10d),0d>') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>'), scircle( '<(280d, -9d),0d>') );
SELECT soverlaps( scircle( '<(280d,-10d),2d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,  0d),2d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT snotoverlaps( scircle( '<(280d,-10d),2d>') ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT snotoverlaps( scircle( '<(280d,  0d),2d>') ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sellipse('<{10d,5d},(280d,-20d),90d>') , scircle( '<(280d,-10d),2d>') );
SELECT soverlaps( sellipse('<{10d,5d},(280d,-20d),90d>') , scircle( '<(280d,  0d),2d>') );
SELECT snotoverlaps( sellipse('<{10d,5d},(280d,-20d),90d>'), scircle( '<(280d,-10d),2d>') );
SELECT snotoverlaps( sellipse('<{10d,5d},(280d,-20d),90d>'), scircle( '<(280d,  0d),2d>') );
SELECT soverlaps( scircle( '<(280d,-10d),0d>')  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
 
--  ellipse is circle
SELECT srcontainsl( scircle( '<(280d,-10d),2d>')  ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-18d),2d>')  ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-16d),2d>')  ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-18d),15d>') ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sellipse('<{5d,5d},(280d,-20d),90d>'),  scircle( '<(280d,-10d),2d>')   );
SELECT srcontainsl( sellipse('<{5d,5d},(280d,-20d),90d>'),  scircle( '<(280d,-18d),2d>')   );
SELECT srcontainsl( sellipse('<{5d,5d},(280d,-20d),90d>'),  scircle( '<(280d,-16d),2d>')   );
SELECT srcontainsl( sellipse('<{5d,5d},(280d,-20d),90d>'),  scircle( '<(280d,-18d),15d>')  );
SELECT soverlaps( scircle( '<(280d,-10d),2d>') ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,-18d),2d>') ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,-16d),2d>') ,  sellipse('<{5d,5d},(280d,-20d),90d>') );

--  ellipse is line
SELECT srcontainsl( scircle( '<(280d,-10d),0d>')  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-18d),0d>')  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-16d),0d>')  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-18d),15d>') ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,-10d),0d>') ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,-18d),0d>') ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-10d),2d>')  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-18d),2d>')  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( scircle( '<(280d,-16d),2d>')  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,-10d),2d>') ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT soverlaps( scircle( '<(280d,-18d),2d>') ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( sellipse('<{5d,0d},(280d,-20d),90d>'), scircle( '<(280d,-18d),15d>')   );

-- 
-- ellipse and line (@,&&)
-- 
--  negators and commutators

SELECT srcontainsl( sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') )  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') )  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') )  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srnotcontainsl( sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srnotcontainsl( sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srnotcontainsl( sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT snotoverlaps( sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ),  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT snotoverlaps( sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ),  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT snotoverlaps( sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ),  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>')  ,  sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ) );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>')  ,  sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ) );
SELECT slcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>')  ,  sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ) );
SELECT soverlaps( sellipse('<{10d,5d},(280d,-20d),90d>')  ,  sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ) );
SELECT soverlaps( sellipse('<{10d,5d},(280d,-20d),90d>')  ,  sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ) );
SELECT soverlaps( sellipse('<{10d,5d},(280d,-20d),90d>')  ,  sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ) );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>') ,  sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ) );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>') ,  sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ) );
SELECT slnotcontainsr( sellipse('<{10d,5d},(280d,-20d),90d>') ,  sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ) );
SELECT snotoverlaps( sellipse('<{10d,5d},(280d,-20d),90d>') ,  sline ( spoint('(280d, -9d)'), spoint('(280d, -8d)') ) );
SELECT snotoverlaps( sellipse('<{10d,5d},(280d,-20d),90d>') ,  sline ( spoint('(280d, -9d)'), spoint('(280d,-12d)') ) );
SELECT snotoverlaps( sellipse('<{10d,5d},(280d,-20d),90d>') ,  sline ( spoint('(280d,-11d)'), spoint('(280d,-12d)') ) );

--  line is point , ellipse is point
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') )  ,  sellipse('<{0d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') )  ,  sellipse('<{0d,0d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') ) ,  sellipse('<{0d,0d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') ) ,  sellipse('<{0d,0d},(280d, -8d),90d>') );
--  line is point , ellipse is circle
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') )  ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') )  ,  sellipse('<{5d,5d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') ) ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') ) ,  sellipse('<{5d,5d},(280d, -8d),90d>') );
--  line is point , ellipse is a real ellipse
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') )  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') )  ,  sellipse('<{10d,5d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -8d)') ) ,  sellipse('<{10d,5d},(280d, -8d),90d>') );
--  line is a real line , ellipse is point
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{0d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{0d,0d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{0d,0d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{0d,0d},(280d, -8d),90d>') );
--  line is a real line , ellipse is circle
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{5d,5d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{5d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{5d,5d},(280d, -8d),90d>') );
--  line is a real line , ellipse is line
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{5d,0d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{5d,0d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{5d,0d},(280d, -8d),90d>') );
--  line is a real line , ellipse is a real ellipse
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT srcontainsl( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') )  ,  sellipse('<{10d,5d},(280d, -8d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{10d,5d},(280d,-20d),90d>') );
SELECT soverlaps( sline ( spoint('(280d, -8d)'), spoint('(280d, -9d)') ) ,  sellipse('<{10d,5d},(280d, -8d),90d>') );


-- 
--  ellipse and ellipse
-- 
  
--  check @
SELECT srcontainsl( sellipse('<{10d, 5d},(300d,0d),  0d>'), sellipse('<{10d,5d},(300d,0d),0d>') );
SELECT srcontainsl( sellipse('<{10d, 5d},(300d,0d), 90d>'), sellipse('<{10d,5d},(300d,0d),0d>') );
SELECT srcontainsl( sellipse('<{10d, 5d},(300d,0d),180d>'), sellipse('<{10d,5d},(300d,0d),0d>') );
SELECT srcontainsl( sellipse('<{10d, 5d},(300d,0d),270d>'), sellipse('<{10d,5d},(300d,0d),0d>') );
SELECT srcontainsl( sellipse('<{ 2d, 1d},( 52d,6d),  0d>'), sellipse('<{10d,5d},(50d,5d),45d>') );

--  negators , commutator @,&&
SELECT srcontainsl( sellipse('<{ 2d, 1d},( 52d,6d),  0d>')  , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT srcontainsl( sellipse('<{10d, 5d},( 52d,6d),  0d>')  , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT srcontainsl( sellipse('<{10d, 5d},( 90d,9d),  0d>')  , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT soverlaps( sellipse('<{ 2d, 1d},( 52d,6d),  0d>') , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT soverlaps( sellipse('<{10d, 5d},( 52d,6d),  0d>') , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT soverlaps( sellipse('<{10d, 5d},( 90d,9d),  0d>') , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT srnotcontainsl( sellipse('<{ 2d, 1d},( 52d,6d),  0d>') , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT srnotcontainsl( sellipse('<{10d, 5d},( 52d,6d),  0d>') , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT srnotcontainsl( sellipse('<{10d, 5d},( 90d,9d),  0d>') , sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT snotoverlaps( sellipse('<{ 2d, 1d},( 52d,6d),  0d>'), sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT snotoverlaps( sellipse('<{10d, 5d},( 52d,6d),  0d>'), sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT snotoverlaps( sellipse('<{10d, 5d},( 90d,9d),  0d>'), sellipse('<{10d,5d},(50d,5d),45d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(50d,5d),45d>') , sellipse('<{ 2d, 1d},( 52d,6d),  0d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(50d,5d),45d>') , sellipse('<{10d, 5d},( 52d,6d),  0d>') );
SELECT slcontainsr( sellipse('<{10d,5d},(50d,5d),45d>') , sellipse('<{10d, 5d},( 90d,9d),  0d>') );
SELECT soverlaps( sellipse('<{10d,5d},(50d,5d),45d>') , sellipse('<{ 2d, 1d},( 52d,6d),  0d>') );
SELECT soverlaps( sellipse('<{10d,5d},(50d,5d),45d>') , sellipse('<{10d, 5d},( 52d,6d),  0d>') );
SELECT soverlaps( sellipse('<{10d,5d},(50d,5d),45d>') , sellipse('<{10d, 5d},( 90d,9d),  0d>') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(50d,5d),45d>'), sellipse('<{ 2d, 1d},( 52d,6d),  0d>') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(50d,5d),45d>'), sellipse('<{10d, 5d},( 52d,6d),  0d>') );
SELECT slnotcontainsr( sellipse('<{10d,5d},(50d,5d),45d>'), sellipse('<{10d, 5d},( 90d,9d),  0d>') );
SELECT snotoverlaps( sellipse('<{10d,5d},(50d,5d),45d>'), sellipse('<{ 2d, 1d},( 52d,6d),  0d>') );
SELECT snotoverlaps( sellipse('<{10d,5d},(50d,5d),45d>'), sellipse('<{10d, 5d},( 52d,6d),  0d>') );
SELECT snotoverlaps( sellipse('<{10d,5d},(50d,5d),45d>'), sellipse('<{10d, 5d},( 90d,9d),  0d>') );

--  left ellipse is point         right ellipse is point
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d,-5d),  0d>')  , sellipse('<{0d,0d},(50d,-5d),45d>') );
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d,-6d),  0d>')  , sellipse('<{0d,0d},(50d,-5d),45d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d,-5d),  0d>') , sellipse('<{0d,0d},(50d,-5d),45d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d,-6d),  0d>') , sellipse('<{0d,0d},(50d,-5d),45d>') );

--  left ellipse is line          right ellipse is point
SELECT srcontainsl( sellipse('<{ 5d, 0d},( 50d, -2d), 90d>')  , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT srcontainsl( sellipse('<{ 5d, 0d},( 50d,-12d), 90d>')  , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT soverlaps( sellipse('<{ 5d, 0d},( 50d, -2d), 90d>') , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT soverlaps( sellipse('<{ 5d, 0d},( 50d,-12d), 90d>') , sellipse('<{0d,0d},(50d,-5d),0d>') );

--  left ellipse is circle        right ellipse is point
SELECT srcontainsl( sellipse('<{ 5d, 5d},( 50d, -2d), 90d>')  , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT srcontainsl( sellipse('<{ 5d, 5d},( 50d,-12d), 90d>')  , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT soverlaps( sellipse('<{ 5d, 5d},( 50d, -2d), 90d>') , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT soverlaps( sellipse('<{ 5d, 5d},( 50d,-12d), 90d>') , sellipse('<{0d,0d},(50d,-5d),0d>') );

--  left ellipse is real ellipse  right ellipse is point
SELECT srcontainsl( sellipse('<{ 5d, 3d},( 50d, -2d), 90d>')  , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT srcontainsl( sellipse('<{ 5d, 3d},( 50d,-12d), 90d>')  , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT soverlaps( sellipse('<{ 5d, 3d},( 50d, -2d), 90d>') , sellipse('<{0d,0d},(50d,-5d),0d>') );
SELECT soverlaps( sellipse('<{ 5d, 3d},( 50d,-12d), 90d>') , sellipse('<{0d,0d},(50d,-5d),0d>') );

--  left ellipse is point         right ellipse is line
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d,-5d),  0d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d,-6d),  0d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d, 6d),  0d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d,-5d),  0d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d,-6d),  0d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d, 6d),  0d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );

--  left ellipse is line         right ellipse is line
SELECT srcontainsl( sellipse('<{ 3d, 0d},( 50d,-5d), 90d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 3d, 0d},( 50d,-8d), 90d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 3d, 0d},( 50d,-6d),  0d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 3d, 0d},( 50d, 6d), 90d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 0d},( 50d,-5d), 90d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 0d},( 50d,-8d), 90d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 0d},( 50d,-6d),  0d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 0d},( 50d, 6d), 90d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );

--  left ellipse is circle         right ellipse is line
SELECT srcontainsl( sellipse('<{ 3d, 3d},( 50d,-5d), 90d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 3d, 3d},( 50d,-8d), 90d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 3d, 3d},( 50d,-6d),  0d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 3d, 3d},( 50d, 6d), 90d>')  , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 3d},( 50d,-5d), 90d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 3d},( 50d,-8d), 90d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 3d},( 50d,-6d),  0d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 3d},( 50d, 6d), 90d>') , sellipse('<{5d,0d},(50d,-5d),90d>') );

--  left ellipse is point         right ellipse is circle
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d,-5d),  0d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d,-6d),  0d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 0d, 0d},( 50d, 6d),  0d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d,-5d),  0d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d,-6d),  0d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 0d, 0d},( 50d, 6d),  0d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );

--  left ellipse is line         right ellipse is circle
SELECT srcontainsl( sellipse('<{ 5d, 0d},( 50d,-5d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 5d, 0d},( 50d,-6d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 5d, 0d},( 50d, 6d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 5d, 0d},( 50d,-5d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 5d, 0d},( 50d,-6d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 5d, 0d},( 50d, 6d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );

--  left ellipse is circle       right ellipse is circle
SELECT srcontainsl( sellipse('<{ 3d, 3d},( 50d,-4d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 5d, 5d},( 50d,-5d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 5d, 5d},( 50d,-6d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT srcontainsl( sellipse('<{ 5d, 5d},( 50d, 6d), 90d>')  , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 3d, 3d},( 50d,-4d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 5d, 5d},( 50d,-5d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 5d, 5d},( 50d,-6d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );
SELECT soverlaps( sellipse('<{ 5d, 5d},( 50d, 6d), 90d>') , sellipse('<{5d,5d},(50d,-5d),90d>') );

