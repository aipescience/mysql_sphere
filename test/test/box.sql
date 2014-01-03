SET sphere_outprec=8;

--
-- sbox ( and spoint

--

SELECT srcontainsl( spoint ( '(0d,90d)' ), sbox ( '((0d,80d),(360d,90d))' ) );

SELECT slcontainsr( sbox ( '((309d,309d),(313d,313d))' ), spoint ( '(310d,310d)' ) );
SELECT srcontainsl( spoint ( '(310d,310d)' ), sbox ( '((309d,309d),(313d,313d))' ) );
SELECT slcontainsr( sbox ( '((309d,309d),(313d,313d))' ), spoint ( '(10d,10d)' ) );
SELECT srcontainsl( spoint ( '(10d,10d)' ), sbox ( '((309d,309d),(313d,313d))' ) );

SELECT slnotcontainsr( sbox ( '((309d,309d),(313d,313d))' ), spoint ( '(310d,310d)' )  );
SELECT srnotcontainsl( spoint ( '(310d,310d)' ), sbox ( '((309d,309d),(313d,313d))' )  );
SELECT slnotcontainsr( sbox ( '((309d,309d),(313d,313d))' ), spoint ( '(10d,10d)' )  );
SELECT srnotcontainsl( spoint ( '(10d,10d)' ), sbox ( '((309d,309d),(313d,313d))' )  );

SELECT slcontainsr( sbox ( '((10d,10d),(20d,20d))' ), spoint ( '(10d,10d)' ));
SELECT slcontainsr( sbox ( '((10d,10d),(20d,20d))' ), spoint ( '(10d,20d)' ));
SELECT slcontainsr( sbox ( '((10d,10d),(20d,20d))' ), spoint ( '(20d,10d)' ));
SELECT slcontainsr( sbox ( '((10d,10d),(20d,20d))' ), spoint ( '(20d,20d)' ));

SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(310d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(310d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(320d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(320d,20d)' ));

SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(310d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(315d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(315d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ), spoint ( '(320d,15d)' ));

SELECT srcontainsl( spoint ( '(320d, 15d)' ), sbox ( '((310d,10d),(320d,20d))' ) );
SELECT srcontainsl( spoint ( '(140d,-15d)' ), sbox ( '((310d,10d),(320d,20d))' ) );

SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(300d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(300d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(300d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(330d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(330d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(330d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(0d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(0d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(0d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(10d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(10d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(10d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(30d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(30d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(30d,20d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(180d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(180d,15d)' ));
SELECT slcontainsr( sbox ( '((310d,10d),(20d,20d))' ), spoint ( '(180d,20d)' ));

SELECT slcontainsr( sbox ( '((310d,-90d),(20d,-80d))' ), spoint ( '(300d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,-90d),(20d,-80d))' ), spoint ( '(320d,-80d)' ));
SELECT slcontainsr( sbox ( '((310d,-90d),(20d,-80d))' ), spoint ( '(320d,-85d)' ));
SELECT slcontainsr( sbox ( '((310d,-90d),(20d,-80d))' ), spoint ( '(320d,-90d)' ));
SELECT slcontainsr( sbox ( '((310d,-90d),(20d,-80d))' ), spoint ( '(180d,-90d)' ));
SELECT slcontainsr( sbox ( '((310d,-90d),(20d,-80d))' ), spoint ( '(180d,-89.99d)' ));

SELECT slcontainsr( sbox ( '((310d,80d),(20d,90d))' ), spoint ( '(300d,10d)' ));
SELECT slcontainsr( sbox ( '((310d,80d),(20d,90d))' ), spoint ( '(320d,80d)' ));
SELECT slcontainsr( sbox ( '((310d,80d),(20d,90d))' ), spoint ( '(320d,85d)' ));
SELECT slcontainsr( sbox ( '((310d,80d),(20d,90d))' ), spoint ( '(320d,90d)' ));
SELECT slcontainsr( sbox ( '((310d,80d),(20d,90d))' ), spoint ( '(180d,90d)' ));
SELECT slcontainsr( sbox ( '((310d,80d),(20d,90d))' ), spoint ( '(180d,89.99d)' ));

SELECT slcontainsr( sbox ( '((0d,-90d),(360d,-80d))' ), spoint ( '(300d,10d)' ));
SELECT slcontainsr( sbox ( '((0d,-90d),(360d,-80d))' ), spoint ( '(320d,-80d)' ));
SELECT slcontainsr( sbox ( '((0d,-90d),(360d,-80d))' ), spoint ( '(320d,-85d)' ));
SELECT slcontainsr( sbox ( '((0d,-90d),(360d,-80d))' ), spoint ( '(320d,-90d)' ));
SELECT slcontainsr( sbox ( '((0d,-90d),(360d,-80d))' ), spoint ( '(180d,-90d)' ));
SELECT slcontainsr( sbox ( '((0d,-90d),(360d,-80d))' ), spoint ( '(180d,-89.99d)' ));

SELECT slcontainsr( sbox ( '((0d,80d),(360d,90d))' ), spoint ( '(300d,10d)' ));
SELECT slcontainsr( sbox ( '((0d,80d),(360d,90d))' ), spoint ( '(320d,80d)' ));
SELECT slcontainsr( sbox ( '((0d,80d),(360d,90d))' ), spoint ( '(320d,85d)' ));
SELECT slcontainsr( sbox ( '((0d,80d),(360d,90d))' ), spoint ( '(320d,90d)' ));
SELECT slcontainsr( sbox ( '((0d,80d),(360d,90d))' ), spoint ( '(180d,90d)' ));
SELECT slcontainsr( sbox ( '((0d,80d),(360d,90d))' ), spoint ( '(180d,89.99d)' ));

SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d,-11d)' ));
SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d,-10d)' ));
SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d, -5d)' ));
SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d,  0d)' ));
SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d,  5d)' ));
SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d, 20d)' ));
SELECT slcontainsr( sbox ( '((0d,-10d),(360d,20d))' ), spoint ( '(270d, 21d)' ));

SELECT srnotcontainsl( spoint ( '(320d, 15d)' ), sbox ( '((310d,10d),(320d,20d))' )  );
SELECT srnotcontainsl( spoint ( '(140d,-15d)' ), sbox ( '((310d,10d),(320d,20d))' )  );

SELECT slnotcontainsr( sbox ( '((10d,10d),(20d,20d))' ), spoint ( '(10d,10d)' ) );
SELECT slnotcontainsr( sbox ( '((270d,-10d),(300d,20d))' ), spoint ( '(10d,10d)' ) );

--
-- sbox ( (as point)  and spoint
--

SELECT slcontainsr( sbox ( '((310d,310d),(310d,310d))' ), spoint ( '(310d,310d)' ) );
SELECT srcontainsl( spoint ( '(310d,310d)' ), sbox ( '((310d,310d),(310d,310d))' ) );
SELECT slcontainsr( sbox ( '((310d,310d),(310d,310d))' ), spoint ( '(10d,10d)' ) );
SELECT srcontainsl( spoint ( '(11d,11d)' ), sbox ( '((310d,310d),(310d,310d))' ) );

--
-- sbox ( and circle
--

-- Check negators / commutators

SELECT soverlaps( scircle ('<(0d, 70d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT soverlaps( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 70d),1d>' ));
SELECT snotoverlaps( scircle ('<(0d, 70d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT snotoverlaps( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 70d),1d>' ));

SELECT soverlaps( scircle ('<(0d, 50d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT soverlaps( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 50d),1d>' ));
SELECT snotoverlaps( scircle ('<(0d, 50d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT snotoverlaps( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 50d),1d>' ));

SELECT srcontainsl( scircle ('<(0d, 70d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT srcontainsl( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 70d),1d>' ));
SELECT srnotcontainsl( scircle ('<(0d, 70d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ) );
SELECT srnotcontainsl( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 70d),1d>' ) );

SELECT srcontainsl( scircle ('<(0d, 50d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT srcontainsl( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 50d),1d>' ));
SELECT srnotcontainsl( scircle ('<(0d, 50d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ) );
SELECT srnotcontainsl( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 50d),1d>' ) );

SELECT slcontainsr( scircle ('<(0d, 70d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT slcontainsr( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 70d),1d>' ));
SELECT slnotcontainsr( scircle ('<(0d, 70d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ) );
SELECT slnotcontainsr( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 70d),1d>' ) );

SELECT slcontainsr( scircle ('<(0d, 50d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ));
SELECT slcontainsr( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 50d),1d>' ));
SELECT slnotcontainsr( scircle ('<(0d, 50d),1d>' ), sbox ( '((-10d, 60d),(10d, 80d))' ) );
SELECT slnotcontainsr( sbox ( '((-10d, 60d),(10d, 80d))' ), scircle ('<(0d, 50d),1d>' ) );


-- Other Checks

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(360d, 90d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(360d,-90d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(360d, 90d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(360d,-90d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(360d, 89d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(360d,-89d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(360d, 89d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(360d,-89d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(360d, 88d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(360d,-88d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(360d, 88d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(360d,-88d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(270d, 90d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(270d,-90d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(270d, 90d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(270d,-90d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(270d, 89d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(270d,-89d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(270d, 89d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(270d,-89d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(270d, 88d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(270d,-88d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(270d, 88d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(270d,-88d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(90d, 90d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(90d,-90d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(90d, 90d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(90d,-90d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(90d, 89d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(90d,-89d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(90d, 89d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(90d,-89d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),1d>' ) , sbox ( '((0d, 80d),(90d, 88d))' )); 
SELECT srcontainsl( scircle ('<(0d,-90d),1d>' ) , sbox ( '((0d,-80d),(90d,-88d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),1d>' ), sbox ( '((0d, 80d),(90d, 88d))' )); 
SELECT soverlaps( scircle ('<(0d,-90d),1d>' ), sbox ( '((0d,-80d),(90d,-88d))' )); 

SELECT srcontainsl( scircle ('<(0d, 89d),1d>' ) , sbox ( '((0d, 80d),(90d, 90d))' )); 
SELECT srcontainsl( scircle ('<(0d,-89d),1d>' ) , sbox ( '((0d,-80d),(90d,-90d))' )); 
SELECT soverlaps( scircle ('<(0d, 89d),1d>' ), sbox ( '((0d, 80d),(90d, 90d))' )); 
SELECT soverlaps( scircle ('<(0d,-89d),1d>' ), sbox ( '((0d,-80d),(90d,-90d))' )); 

SELECT srcontainsl( scircle ('<(0d, 89d),1d>' ) , sbox ( '((0d, 80d),(90d, 89d))' )); 
SELECT srcontainsl( scircle ('<(0d,-89d),1d>' ) , sbox ( '((0d,-80d),(90d,-89d))' )); 
SELECT soverlaps( scircle ('<(0d, 89d),1d>' ), sbox ( '((0d, 80d),(90d, 89d))' )); 
SELECT soverlaps( scircle ('<(0d,-89d),1d>' ), sbox ( '((0d,-80d),(90d,-89d))' )); 

SELECT srcontainsl( scircle ('<(0d, 89d),1d>' ) , sbox ( '((0d, 80d),(90d, 88d))' )); 
SELECT srcontainsl( scircle ('<(0d,-89d),1d>' ) , sbox ( '((0d,-80d),(90d,-88d))' )); 
SELECT soverlaps( scircle ('<(0d, 89d),1d>' ), sbox ( '((0d, 80d),(90d, 88d))' )); 
SELECT soverlaps( scircle ('<(0d,-89d),1d>' ), sbox ( '((0d,-80d),(90d,-88d))' )); 

SELECT srcontainsl( scircle ('<(0d, 90d),10d>' ) , sbox ( '((0d, 80d),(360d, 90d))' ));
SELECT slcontainsr( scircle ('<(0d, 90d),10d>' ) , sbox ( '((0d, 80d),(360d, 90d))' )); 
SELECT soverlaps( scircle ('<(0d, 90d),10d>' ) , sbox ( '((0d, 80d),(360d, 90d))' ));


--
-- sbox ( and line
--

SELECT srcontainsl( sline( spoint ( '(0d,0d)' ), spoint ( '(0d,10d)' ) ), sbox ( '((0d,0d),(10d,10d))' ));
SELECT srcontainsl( sline( spoint ( '(0d,0d)' ), spoint ( '(0d,0d)' ) ) , sbox ( '((0d,0d),(10d,10d))' ));
SELECT srcontainsl( sline( spoint ( '(0d,10d)' ), spoint ( '(10d,10d)' ) ), sbox ( '((0d,0d),(10d,10d))' ));

SELECT soverlaps( sline( spoint ( '(0d,0d)' ), spoint ( '(0d,10d)' ) ), sbox ( '((0d,0d),(10d,10d))' ));
SELECT soverlaps( sline( spoint ( '(0d,0d)' ), spoint ( '(0d,0d)' ) ) , sbox ( '((0d,0d),(10d,10d))' ));
SELECT soverlaps( sline( spoint ( '(0d,10d)' ), spoint ( '(10d,10d)' ) ), sbox ( '((0d,0d),(10d,10d))' ));

-- sbox ( is point, sline is point

SELECT slcontainsr( sbox ( '((310d,310d),(310d,310d))' ), sline ( spoint ( '(310d,310d)' ), spoint ( '(310d,310d)' ) ) );
SELECT srcontainsl( sline ( spoint ( '(310d,310d)' ), spoint ( '(310d,310d)' ) ), sbox ( '((310d,310d),(310d,310d))' ) );
SELECT slcontainsr( sbox ( '((310d,310d),(310d,310d))' ), sline ( spoint ( '(10d,10d)' ), spoint ( '(10d,10d)' ) ) );
SELECT srcontainsl( sline ( spoint ( '(11d,11d)' ) , spoint ( '(11d,11d)' ) ), sbox ( '((310d,310d),(310d,310d))' ) );
SELECT srcontainsl( sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,90d)' ) ), sbox ( '((310d,90d),(310d,90d))' ) );
SELECT slcontainsr( sbox ( '((310d,90d),(310d,90d))' ), sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,90d)' ) ) );

SELECT soverlaps( sbox ( '((310d,310d),(310d,310d))' ), sline ( spoint ( '(310d,310d)' ), spoint ( '(310d,310d)' ) ) );
SELECT soverlaps( sline ( spoint ( '(310d,310d)' ), spoint ( '(310d,310d)' ) ), sbox ( '((310d,310d),(310d,310d))' ) );
SELECT soverlaps( sbox ( '((310d,310d),(310d,310d))' ), sline ( spoint ( '(10d,10d)' ), spoint ( '(10d,10d)' ) ) );
SELECT soverlaps( sline ( spoint ( '(11d,11d)' ) , spoint ( '(11d,11d)' ) ), sbox ( '((310d,310d),(310d,310d))' ) );
SELECT soverlaps( sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,90d)' ) ), sbox ( '((310d,90d),(310d,90d))' ) );
SELECT soverlaps( sbox ( '((310d,90d),(310d,90d))' ), sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,90d)' ) ) );


-- sbox ( is point only

SELECT slcontainsr( sbox ( '((310d,10d),(310d,10d))' ) , sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,20d)' ) ) );
SELECT soverlaps( sbox ( '((310d,10d),(310d,10d))' ), sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,20d)' ) ) );
SELECT srcontainsl( sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,20d)' ) )  , sbox ( '((310d,10d),(310d,10d))' ) );
SELECT soverlaps( sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,20d)' ) ) , sbox ( '((310d,10d),(310d,10d))' ) );
SELECT slcontainsr( sbox ( '((310d,10d),(310d,10d))' ) , sline ( spoint ( '(10d,10d)' ), spoint ( '(10d,20d)' ) ) );
SELECT soverlaps( sbox ( '((310d,10d),(310d,10d))' ), sline ( spoint ( '(10d,10d)' ), spoint ( '(10d,20d)' ) ) );
SELECT srcontainsl( sline ( spoint ( '(11d,11d)' ) , spoint ( '(21d,11d)' ) ) , sbox ( '((310d,10d),(310d,10d))' ) );
SELECT soverlaps( sline ( spoint ( '(11d,11d)' ) , spoint ( '(21d,11d)' ) ), sbox ( '((310d,10d),(310d,10d))' ) );
SELECT srcontainsl( sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,80d)' ) ) , sbox ( '((310d,90d),(310d,90d))' ) );
SELECT soverlaps( sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,80d)' ) ), sbox ( '((310d,90d),(310d,90d))' ) );
SELECT slcontainsr( sbox ( '((310d,90d),(310d,90d))' ) , sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,80d)' ) ) );
SELECT soverlaps( sbox ( '((310d,90d),(310d,90d))' ), sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,80d)' ) ) );

-- sline is point only

SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ) , sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,10d)' ) ) );
SELECT soverlaps( sbox ( '((310d,10d),(320d,20d))' ), sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,10d)' ) ) );
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ) , sline ( spoint ( '(315d,15d)' ), spoint ( '(315d,15d)' ) ) );
SELECT soverlaps( sbox ( '((310d,10d),(320d,20d))' ), sline ( spoint ( '(315d,15d)' ), spoint ( '(315d,15d)' ) ) );
SELECT srcontainsl( sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,10d)' ) )  , sbox ( '((310d,10d),(320d,20d))' ) );
SELECT soverlaps( sline ( spoint ( '(310d,10d)' ), spoint ( '(310d,10d)' ) ) , sbox ( '((310d,10d),(320d,20d))' ) );
SELECT srcontainsl( sline ( spoint ( '(315d,15d)' ), spoint ( '(315d,15d)' ) )  , sbox ( '((310d,10d),(320d,20d))' ) );
SELECT soverlaps( sline ( spoint ( '(315d,15d)' ), spoint ( '(315d,15d)' ) ) , sbox ( '((310d,10d),(320d,20d))' ) );
SELECT slcontainsr( sbox ( '((310d,10d),(320d,20d))' ) , sline ( spoint ( '(10d,10d)' ), spoint ( '(10d,10d)' ) ) );
SELECT soverlaps( sbox ( '((310d,10d),(320d,20d))' ), sline ( spoint ( '(10d,10d)' ), spoint ( '(10d,10d)' ) ) );
SELECT srcontainsl( sline ( spoint ( '(11d, 11d)' ) , spoint ( '(11d,11d)' ) ) , sbox ( '((310d,10d),(320d,20d))' ) );
SELECT soverlaps( sline ( spoint ( '(11d, 11d)' ) , spoint ( '(11d,11d)' ) ), sbox ( '((310d,10d),(320d,20d))' ) );
SELECT srcontainsl( sline ( spoint ( '(11d, 90d)' ) , spoint ( '(11d,90d)' ) ) , sbox ( '((310d,80d),(320d,90d))' ) );
SELECT soverlaps( sline ( spoint ( '(11d, 90d)' ) , spoint ( '(11d,90d)' ) ), sbox ( '((310d,80d),(320d,90d))' ) );
SELECT srcontainsl( sline ( spoint ( '(11d, 85d)' ) , spoint ( '(11d,85d)' ) ) , sbox ( '((310d,80d),(320d,90d))' ) );
SELECT soverlaps( sline ( spoint ( '(11d, 85d)' ) , spoint ( '(11d,85d)' ) ), sbox ( '((310d,80d),(320d,90d))' ) );
SELECT srcontainsl( sline ( spoint ( '(315d,85d)' ) , spoint ( '(315d,85d)' ) ) , sbox ( '((310d,80d),(320d,90d))' ) );
SELECT soverlaps( sline ( spoint ( '(315d,85d)' ) , spoint ( '(315d,85d)' ) ), sbox ( '((310d,80d),(320d,90d))' ) );
SELECT slcontainsr( sbox ( '((310d,80d),(320d,90d))' ) , sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,90d)' ) ) );
SELECT soverlaps( sbox ( '((310d,80d),(320d,90d))' ), sline ( spoint ( '(11d,90d)' ) , spoint ( '(11d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((310d,80d),(320d,90d))' ) , sline ( spoint ( '(315d,85d)' ) , spoint ( '(315d,85d)' ) ) );
SELECT soverlaps( sbox ( '((310d,80d),(320d,90d))' ), sline ( spoint ( '(315d,85d)' ) , spoint ( '(315d,85d)' ) ) );

-- sline / sbox ( near pole

SELECT slcontainsr( sbox ( '((20d,80d),(200d,90d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,85d)' ) ) );
SELECT soverlaps( sbox ( '((20d,80d),(200d,90d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,85d)' ) ) );
SELECT slcontainsr( sbox ( '((10d,80d),(210d,90d))' ) , sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,81d)' ) ) );
SELECT soverlaps( sbox ( '((10d,80d),(210d,90d))' ), sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,81d)' ) ) );
SELECT slcontainsr( sbox ( '((10d,80d),(211d,90d))' ) , sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,81d)' ) ) );
SELECT soverlaps( sbox ( '((10d,80d),(211d,90d))' ), sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,81d)' ) ) );


SELECT slcontainsr( sbox ( '((190d,80d),(199d,90d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((190d,80d),(199d,90d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((190d,80d),(200d,90d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((190d,80d),(200d,90d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((190d,80d),(210d,90d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((190d,80d),(210d,90d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,80d),(210d,90d))' ) , sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((200d,80d),(210d,90d))' ), sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((210d,80d),(220d,90d))' ) , sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((210d,80d),(220d,90d))' ), sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );

SELECT slcontainsr( sbox ( '((190d,80d),(199d,88d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((190d,80d),(199d,88d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((190d,80d),(200d,88d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((190d,80d),(200d,88d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((190d,80d),(210d,88d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((190d,80d),(210d,88d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,80d),(210d,88d))' ) , sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((200d,80d),(210d,88d))' ), sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );
SELECT slcontainsr( sbox ( '((210d,80d),(220d,88d))' ) , sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );
SELECT soverlaps( sbox ( '((210d,80d),(220d,88d))' ), sline ( spoint ( '(200d,81d)' ), spoint ( '(20d,90d)' ) ) );

-- sbox ( near equator

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d,85d)' ), spoint ( '( 20d, 85d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d,85d)' ), spoint ( '( 20d, 85d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(190d,  0d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(190d,  0d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(205d,  0d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(205d,  0d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(202d, 0d)' ), spoint ( '(207d,  0d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(202d, 0d)' ), spoint ( '(207d,  0d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d,-5d)' ), spoint ( '(200d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d,-5d)' ), spoint ( '(200d,  5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(205d,-5d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(205d,-5d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(200d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(200d,  5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(200d, 10d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(200d, 10d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(205d, 0d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(205d, 0d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(205d, 0d)' ), spoint ( '(205d, 10d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(205d, 0d)' ), spoint ( '(205d, 10d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d,-5d)' ), spoint ( '(200d, 15d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d,-5d)' ), spoint ( '(200d, 15d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(205d,-5d)' ), spoint ( '(205d, 15d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(205d,-5d)' ), spoint ( '(205d, 15d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(210d, 10d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(210d, 10d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(199d,-1d)' ), spoint ( '(211d, 11d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(199d,-1d)' ), spoint ( '(211d, 11d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(211d, 11d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(211d, 11d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(199d,-1d)' ), spoint ( '(210d, 10d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(199d,-1d)' ), spoint ( '(210d, 10d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(199d,-1d)' ), spoint ( '(209d,  9d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(199d,-1d)' ), spoint ( '(209d,  9d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(201d, 1d)' ), spoint ( '(211d, 11d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(201d, 1d)' ), spoint ( '(211d, 11d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(201d, 0d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(201d, 0d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(201d, 1d)' ), spoint ( '(205d,  5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(201d, 1d)' ), spoint ( '(205d,  5d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d,10d)' ), spoint ( '(210d, 10d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d,10d)' ), spoint ( '(210d, 10d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d,10d)' ), spoint ( '(205d, 10d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d,10d)' ), spoint ( '(205d, 10d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(199d,-1d)' ), spoint ( '(200d,  0d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(199d,-1d)' ), spoint ( '(200d,  0d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d,10d)' ), spoint ( '(200d, 20d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d,10d)' ), spoint ( '(200d, 20d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(205d,10d)' ), spoint ( '(205d, 20d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(205d,10d)' ), spoint ( '(205d, 20d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(210d,10d)' ), spoint ( '(210d, 20d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(210d,10d)' ), spoint ( '(210d, 20d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(200d, 0d)' ), spoint ( '(200d, -5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(200d, 0d)' ), spoint ( '(200d, -5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(205d, 0d)' ), spoint ( '(205d, -5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(205d, 0d)' ), spoint ( '(205d, -5d)' ) ) );
SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ( spoint ( '(210d, 0d)' ), spoint ( '(210d, -5d)' ) ) );
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ( spoint ( '(210d, 0d)' ), spoint ( '(210d, -5d)' ) ) );

SELECT slcontainsr( sbox ( '((200d,0d),(210d,10d))' ) , sline ('( -10d, -10d, 200d, ZXZ ), 20d' ));
SELECT soverlaps( sbox ( '((200d,0d),(210d,10d))' ), sline ('( -10d, -10d, 200d, ZXZ ), 20d' ));

-- general position

SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' )  , sline ( spoint ( '(170d,  0d)' ), spoint ( '(190d,   0d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d,  0d)' ), spoint ( '(190d,   0d)' ) ) );

SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -40d)' ), spoint ( '(190d, -40d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(170d, -40d)' ), spoint ( '(190d, -40d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -50d)' ), spoint ( '(190d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(170d, -50d)' ), spoint ( '(190d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(171d, -40d)' ), spoint ( '(189d, -40d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(171d, -40d)' ), spoint ( '(189d, -40d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(171d, -50d)' ), spoint ( '(189d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(171d, -50d)' ), spoint ( '(189d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(171d, -40d)' ), spoint ( '(191d, -40d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(171d, -40d)' ), spoint ( '(191d, -40d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(171d, -50d)' ), spoint ( '(191d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(171d, -50d)' ), spoint ( '(191d, -50d)' ) ) );

SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -40d)' ), spoint ( '(170d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(170d, -40d)' ), spoint ( '(170d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -40d)' ), spoint ( '(170d, -49d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(170d, -40d)' ), spoint ( '(170d, -49d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -40d)' ), spoint ( '(170d, -51d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(170d, -40d)' ), spoint ( '(170d, -51d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -40d)' ), spoint ( '(190d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(170d, -40d)' ), spoint ( '(190d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(171d, -41d)' ), spoint ( '(189d, -49d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(171d, -41d)' ), spoint ( '(189d, -49d)' ) ) );

SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -39d)' ), spoint ( '(180d, -51d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -39d)' ), spoint ( '(180d, -51d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -40d)' ), spoint ( '(180d, -51d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -40d)' ), spoint ( '(180d, -51d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -40d)' ), spoint ( '(180d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -40d)' ), spoint ( '(180d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -41d)' ), spoint ( '(180d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -41d)' ), spoint ( '(180d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -41d)' ), spoint ( '(180d, -49d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -41d)' ), spoint ( '(180d, -49d)' ) ) );

SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -39d)' ), spoint ( '(182d, -51d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -39d)' ), spoint ( '(182d, -51d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -40d)' ), spoint ( '(182d, -51d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -40d)' ), spoint ( '(182d, -51d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -40d)' ), spoint ( '(182d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -40d)' ), spoint ( '(182d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -41d)' ), spoint ( '(182d, -50d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -41d)' ), spoint ( '(182d, -50d)' ) ) );
SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(180d, -41d)' ), spoint ( '(182d, -49d)' ) ) );
SELECT soverlaps( sbox ( '((170d,-50d),(190d,-40d))' ), sline ( spoint ( '(180d, -41d)' ), spoint ( '(182d, -49d)' ) ) );

SELECT slcontainsr( sbox ( '((170d,-50d),(190d,-40d))' ) , sline ( spoint ( '(170d, -40d)' ), spoint ( '(190d, -40d)' ) ) );

-- wide spherical boxes
