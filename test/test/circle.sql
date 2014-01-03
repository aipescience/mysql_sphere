SET sphere_outprec=8;

-- Input/Output ---

SET sphere_outmode="RAD";

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1.0d >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1d 30m >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 0.1 >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 90d >'));

SET sphere_outmode="DEG";

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1.0d >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1d 30m >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 0.1 >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 90d >'));

SET sphere_outmode="DMS";

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1.0d >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1d 30m >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 0.1 >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 90d >'));

SET sphere_outmode="HMS";

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1.0d >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 1d 30m >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 0.1 >'));

SELECT sstr(scircle( '< (1h 2m 30s , +1d 2m 30s), 90d >'));

SET sphere_outmode="DMS";



-- Functions


-- checking spherical circle operators

SELECT sequal( scircle( '<(0, 90d),1>' )  ,  scircle( '<(0, 90d),1>' ));
SELECT snotequal( scircle( '<(0,-90d),1>') , scircle( '<(0, 90d),1>' ));


-- float8 sdist(scircle,scircle)

SELECT 180.0*sdist(scircle( '<( 0h 2m 30s , 10d 0m 0s), 0.1d>'),scircle( '<( 0h 2m 30s , -10d 0m 0s),0.1d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 0m 00s , 0d 0m 0s), 1.0d>'),scircle( '<( 1h 0m 00s , 0d 0m 0s),1.0d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 23h 30m 00s , 0d 0m 0s), 1.0d>'),scircle( '<( 1h 0m 00s , 0d 0m 0s),1.5d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 0d 0m 0s), 1.0d>'),scircle( '<( 0h 50m 00s , 0d 0m 0s),1.0d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 0d 0m 0s), 1.5d>'),scircle( '<( 0h 50m 00s , 0d 0m 0s),1.5d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 90d 0m 0s), 1.5d>'),scircle( '<( 0h 50m 00s , 0d 0m 0s),1.5d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 90d 0m 0s), 1.0d>'),scircle( '<( 0h 50m 00s , -90d 0m 0s),1.0d>'))/pi();



-- spoint scenter(scircle)

SELECT sstr(scenter(scircle( '< (1h 2m 30s , +1d 2m 30s), 1.0d >')));

SELECT sstr(scenter(scircle( '< (1h 2m 30s , +90d 0m 0s), 1.0d >')));



-- scircle scircle(spoint)

SELECT sstr(scircle(spoint( '(0d,0d)' )));

SELECT sstr(scircle(spoint( '(0d,90d)' )));

SELECT sstr(scircle(spoint( '(0d,-90d)' )));



-- Operators

-- ,  opsequal( erato)r

-- should be "true"

SELECT sequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,+1d), 1.5d >') );

SELECT sequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (25h 0m 0s,+1d), 1d 30m >') );

SELECT sequal( scircle( '< (1h 0m 0s,+95d), 1.5d >'), scircle( '< (13h 0m 0s,+85d), 1.5d >') );

SELECT sequal( scircle( '< (1h 0m 0s,-95d), 1.5d >'), scircle( '< (13h 0m 0s,-85d), 1.5d >') );

SELECT sequal( scircle( '< (1h 0m 0s,+90d), 1.5d >'), scircle( '< (2h 0m 0s,+90d), 1.5d >') );

SELECT sequal( scircle( '< (1h 0m 0s,-90d), 1.5d >'), scircle( '< (2h 0m 0s,-90d), 1.5d >') );

-- should be "false"

SELECT sequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,-1d), 1.5d >') );

SELECT sequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,+1d), 2.5d >') );



-- osnotequal( perato9r

-- should be "false"

SELECT snotequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,+1d), 1.5d >') );

SELECT snotequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (25h 0m 0s,+1d), 1d 30m >') );

SELECT snotequal( scircle( '< (1h 0m 0s,+95d), 1.5d >'), scircle( '< (13h 0m 0s,+85d), 1.5d >') );

SELECT snotequal( scircle( '< (1h 0m 0s,-95d), 1.5d >'), scircle( '< (13h 0m 0s,-85d), 1.5d >') );

SELECT snotequal( scircle( '< (1h 0m 0s,+90d), 1.5d >'), scircle( '< (2h 0m 0s,+90d), 1.5d >') );

SELECT snotequal( scircle( '< (1h 0m 0s,-90d), 1.5d >'), scircle( '< (2h 0m 0s,-90d), 1.5d >') );

-- should be "true"

SELECT snotequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,-1d), 1.5d >') );

SELECT snotequal( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,+1d), 2.5d >') );



-- osoverlaps(perato )r

-- should be "true"

SELECT soverlaps( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,+1d), 1.5d >') );

SELECT soverlaps( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,+1d), 0.5d >') );

SELECT soverlaps( scircle( '< (1h 0m 0s,+1d), 1.5d >'), scircle( '< (1h 0m 0s,-1d), 1.5d >') );

SELECT soverlaps( scircle( '< (1h 0m 0s,0d), 15d >'), scircle( '< (0h 0m 0s,0d), 15d >') );

SELECT soverlaps( scircle( '< (2h 0m 0s,0d), 15d >'), scircle( '< (0h 0m 0.1s,0d), 15d >') );

SELECT soverlaps( scircle( '< (1h 0m 0s,0d), 15d >'), scircle( '< (23h 0m 0.1s,0d), 15d >') );

-- should be "false"

SELECT soverlaps( scircle( '< (1h 0m 0s,+1d), 0.5d >'), scircle( '< (1h 0m 0s,-1d), 0.5d >') );

SELECT soverlaps( scircle( '< (1d 0m 0s,+1d), 1.0d >'), scircle( '< (0d 0m 0s,0d), 0.1d >') );



-- scenter( operato )r

SELECT sstr(scenter( scircle( '< (1h 2m 3s , +1d 2m 3s), 1.0d >') ));

SELECT sstr(scenter( scircle( '< (1h 2m 3s , +90d 0m 0s), 1.0d >') ));



-- ,  sdist( operato)r

SELECT 180.0*sdist(scircle( '<( 0h 2m 30s , 10d 0m 0s), 0.1d>'), scircle( '<( 0h 2m 30s , -10d 0m 0s),0.1d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 0m 00s , 0d 0m 0s), 1.0d>'), scircle( '<( 1h 0m 00s , 0d 0m 0s),1.0d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 23h 30m 00s , 0d 0m 0s), 1.0d>'), scircle( '<( 1h 0m 00s , 0d 0m 0s),1.5d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 0d 0m 0s), 1.0d>'), scircle( '<( 0h 50m 00s , 0d 0m 0s),1.0d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 0d 0m 0s), 1.5d>'), scircle( '<( 0h 50m 00s , 0d 0m 0s),1.5d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 90d 0m 0s), 1.5d>'), scircle( '<( 0h 50m 00s , 0d 0m 0s),1.5d>'))/pi();

SELECT 180.0*sdist(scircle( '<( 0h 40m 00s , 90d 0m 0s), 1.0d>'), scircle( '<( 0h 50m 00s , -90d 0m 0s),1.0d>'))/pi();



-- scirsrcontainsl( cle, scircle operato )r

-- should be "true"

SELECT srcontainsl(  scircle( '< (1h 0m 0s,+1d),  0.5d >'), scircle( '< (1h 0m 0s,+1d),  1.5d >') );

SELECT srcontainsl(  scircle( '< (2d 0m 0s,+1d),  0.5d >'), scircle( '< (1d 0m 0s,0d),   3.5d >') );

SELECT srcontainsl(  scircle( '< (1h 0m 0s,+89d), 0.5d >'), scircle( '< (1h 0m 0s,+90d), 1.5d >') );

SELECT srcontainsl(  scircle( '< (1h 0m 0s,-89d), 0.5d >'), scircle( '< (1h 0m 0s,-90d), 1.5d >') );

-- should be "false"

SELECT srcontainsl(  scircle( '< (1h 0m 0s,+1d),  0.5d >'), scircle( '< (2h 0m 0s,+1d),  1.5d >') );

SELECT srcontainsl(  scircle( '< (2d 0m 0s,+1d),  1.5d >'), scircle( '< (0d 0m 0s, 0d),  3.5d >') );

SELECT srcontainsl(  scircle( '< (1h 0m 0s,+89d), 0.5d >'), scircle( '< (1h 0m 0s,+90d), 1.0d >') );

SELECT srcontainsl(  scircle( '< (1h 0m 0s,-89d), 0.5d >'), scircle( '< (1h 0m 0s,-90d), 1.0d >') );



-- scirslcontainsr( cle, scircle operato )r

-- should be "true"

SELECT slcontainsr( scircle( '< (1h 0m 0s,+1d),  1.5d >') , scircle( '< (1h 0m 0s,+1d),  0.5d >') );

SELECT slcontainsr( scircle( '< (1d 0m 0s,0d),   3.5d >') , scircle( '< (2d 0m 0s,+1d),  0.5d >') );

SELECT slcontainsr( scircle( '< (1h 0m 0s,+90d), 1.5d >') , scircle( '< (1h 0m 0s,+89d), 0.5d >') );

SELECT slcontainsr( scircle( '< (1h 0m 0s,-90d), 1.5d >') , scircle( '< (1h 0m 0s,-89d), 0.5d >') );

-- should be "false"

SELECT slcontainsr( scircle( '< (2h 0m 0s,+1d),  1.5d >') , scircle( '< (1h 0m 0s,+1d),  0.5d >') );

SELECT slcontainsr( scircle( '< (0d 0m 0s, 0d),  3.5d >') , scircle( '< (2d 0m 0s,+1d),  1.5d >') );

SELECT slcontainsr( scircle( '< (1h 0m 0s,+90d), 1.0d >') , scircle( '< (1h 0m 0s,+89d), 0.5d >') );

SELECT slcontainsr( scircle( '< (1h 0m 0s,-90d), 1.0d >') , scircle( '< (1h 0m 0s,-89d), 0.5d >') );



-- spoisrcontainsl( nt, scircle operato )r

-- should be "true"

SELECT srcontainsl(  spoint( '(1h 0m 0s,+1d) ' ), scircle( '< (1h 0m 0s,+1d),  1.5d >') );

SELECT srcontainsl(  spoint( '(2d 0m 0s,+1d) ' ), scircle( '< (1d 0m 0s, 0d),  3.5d >') );

SELECT srcontainsl(  spoint( '(1h 0m 0s,+89d)' ), scircle( '< (1h 0m 0s,+90d), 1.5d >') );

SELECT srcontainsl(  spoint( '(1h 0m 0s,-89d)' ), scircle( '< (1h 0m 0s,-90d), 1.5d >') );

SELECT srcontainsl(  spoint( '(1h 0m 0s,+89d)' ), scircle( '< (1h 0m 0s,+90d), 1.0d >') );

SELECT srcontainsl(  spoint( '(1h 0m 0s,-89d)' ), scircle( '< (1h 0m 0s,-90d), 1.0d >') );

-- should be "false"

SELECT srcontainsl(  spoint( '(1h 0m  0s,+1d    )' ), scircle( '< (2h 0m 0s,-1d),  1.5d >') );

SELECT srcontainsl(  spoint( '(3d 30m 0s,+1d    )' ), scircle( '< (0d 0m 0s, 0d),  3.5d >') );

SELECT srcontainsl(  spoint( '(1h 0m  0s,+88.99d)' ), scircle( '< (1h 0m 0s,+90d), 1.0d >') );

SELECT srcontainsl(  spoint( '(1h 0m  0s,-88.99d)' ), scircle( '< (1h 0m 0s,-90d), 1.0d >') );



-- spoislcontainsr( nt, scircle operato )r

-- should be "true"

SELECT slcontainsr( scircle( '< (1h 0m 0s,+1d),  1.5d >'), spoint( '(1h 0m 0s,+1d) ' ) );

SELECT slcontainsr( scircle( '< (1d 0m 0s, 0d),  3.5d >'), spoint( '(2d 0m 0s,+1d) ' ) );

SELECT slcontainsr( scircle( '< (1h 0m 0s,+90d), 1.5d >'), spoint( '(1h 0m 0s,+89d)' ) );

SELECT slcontainsr( scircle( '< (1h 0m 0s,-90d), 1.5d >'), spoint( '(1h 0m 0s,-89d)' ) );

SELECT slcontainsr( scircle( '< (1h 0m 0s,+90d), 1.0d >'), spoint( '(1h 0m 0s,+89d)' ) );

SELECT slcontainsr( scircle( '< (1h 0m 0s,-90d), 1.0d >'), spoint( '(1h 0m 0s,-89d)' ) );

-- should be "false"

SELECT slcontainsr( scircle( '< (2h 0m 0s,-1d),  1.5d >'), spoint( '(1h 0m  0s,+1d    )' ) );

SELECT slcontainsr( scircle( '< (0d 0m 0s, 0d),  3.5d >'), spoint( '(3d 30m 0s,+1d    )' ) );

SELECT slcontainsr( scircle( '< (1h 0m 0s,+90d), 1.0d >'), spoint( '(1h 0m  0s,+88.99d)' ) );

SELECT slcontainsr( scircle( '< (1h 0m 0s,-90d), 1.0d >'), spoint( '(1h 0m  0s,-88.99d)' ) );
