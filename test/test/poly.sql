USE spheretest;

SET sphere_outprec=8;

-- checking polygon operators

set @poly=( SELECT spoly( '{(0.1,0),(0.2,0),(0.2,0.1),(0.3,0.1),(0.3,-0.1),(0.4,-0.1),(0.5,0.1),(0.4,0.2),(0.1,0.2)}' ) );
SELECT srcontainsl( spoint( '(0.15,0.10)' ), @poly );                  -- point inside polygo )n
SELECT srcontainsl( spoint( '(0.20,0.00)' ), @poly );                  -- point contained polygon ) 
SELECT srcontainsl( spoint( '(0.10,0.10)' ), @poly );                  -- point contained polygon ) 
SELECT srcontainsl( spoint( '(0.25,0.50)' ), @poly );                  -- point outside polygon ) 
SELECT srcontainsl( spoint( '(0.25,0.00)' ), @poly );                  -- point outside polygon ) 
SELECT srcontainsl( scircle( '<(0.15,0.10),0.03>' ),  @poly );          -- circle inside polygo )n
SELECT srcontainsl( scircle( '<(0.20,0.00),0.00>' ),  @poly );          -- circle contained polygon ) 
SELECT srcontainsl( scircle( '<(0.20,0.30),0.05>' ),  @poly );          -- circle outside  polygon ) 
SELECT srcontainsl( scircle( '<(0.25,0.00),0.05>' ),  @poly );          -- circle overlaps polygo )n
SELECT srcontainsl( scircle( '<(0.25,0.00),0.10>' ),  @poly );          -- circle overlaps polygo )n
SELECT soverlaps( scircle( '<(0.15,0.10),0.03>' ), @poly );          -- circle inside polygo )n
SELECT soverlaps( scircle( '<(0.20,0.00),0.00>' ), @poly );          -- circle contained polygon ) 
SELECT soverlaps( scircle( '<(0.20,0.30),0.05>' ), @poly );          -- circle outside  polygon ) 
SELECT soverlaps( scircle( '<(0.25,0.00),0.05>' ), @poly );          -- circle overlaps polygo )n
SELECT soverlaps( scircle( '<(0.25,0.00),0.10>' ), @poly );          -- circle overlaps polygo )n
SELECT srcontainsl( sline ( spoint( '(0.00, 0.00)' ), spoint( '(0.10,0.20)' ) ),  @poly );  -- line touches polygon ) 
SELECT srcontainsl( sline ( spoint( '(0.00, 0.10)' ), spoint( '(0.10,0.10)' ) ),  @poly );  -- line touches polygon ) 
SELECT srcontainsl( sline ( spoint( '(0.50, 0.00)' ), spoint( '(0.50,0.20)' ) ),  @poly );  -- line touches polygo )n
SELECT srcontainsl( sline ( spoint( '(0.10, 0.20)' ), spoint( '(0.20,0.00)' ) ),  @poly );  -- line touches and inside polygon ) 
SELECT srcontainsl( sline ( spoint( '(0.45,-0.20)' ), spoint( '(0.45,0.20)' ) ),  @poly );  -- line overlaps polygo )n
SELECT srcontainsl( sline ( spoint( '(0.45, 0.10)' ), spoint( '(0.45,0.20)' ) ),  @poly );  -- line overlaps polygo )n
SELECT srcontainsl( sline ( spoint( '(0.24, 0.17)' ), spoint( '(0.25,0.14)' ) ),  @poly );  -- line inside  polygo )n
SELECT soverlaps( sline ( spoint( '(0.00, 0.00)' ), spoint( '(0.10,0.20)' ) ), @poly );  -- line touches polygon ) 
SELECT soverlaps( sline ( spoint( '(0.00, 0.10)' ), spoint( '(0.10,0.10)' ) ), @poly );  -- line touches polygon ) 
SELECT soverlaps( sline ( spoint( '(0.50, 0.00)' ), spoint( '(0.50,0.20)' ) ), @poly );  -- line touches polygo )n
SELECT soverlaps( sline ( spoint( '(0.10, 0.20)' ), spoint( '(0.20,0.00)' ) ), @poly );  -- line touches and inside polygon ) 
SELECT soverlaps( sline ( spoint( '(0.45,-0.20)' ), spoint( '(0.45,0.20)' ) ), @poly );  -- line overlaps polygo )n
SELECT soverlaps( sline ( spoint( '(0.45, 0.10)' ), spoint( '(0.45,0.20)' ) ), @poly );  -- line overlaps polygo )n
SELECT soverlaps( sline ( spoint( '(0.24, 0.17)' ), spoint( '(0.25,0.14)' ) ), @poly );  -- line inside  polygo )n

set @poly1=spoly( '{(0,0),(1,0),(0,1)}' );
set @poly2=spoly( '{(1,0),(0,0),(0,1)}' );
set @poly3=spoly( '{(0,1),(0,0),(1,0)}' );
set @poly4=spoly( '{(0.1,0.9),(0.1,0.1),(0.9,0.1)}' );
set @poly5=spoly( '{(0.2,0.0),(1.2,0.0),(0.2,1)}' );

SELECT sequal( @poly1 , @poly2 );
SELECT sequal( @poly2 , @poly3 );
SELECT sequal( @poly3 , @poly1 );
SELECT soverlaps( @poly1, @poly2 );
SELECT soverlaps( @poly2, @poly3 );
SELECT soverlaps( @poly3, @poly1 );
SELECT srcontainsl( @poly1 , @poly2 );
SELECT srcontainsl( @poly2 , @poly3 );
SELECT srcontainsl( @poly3 , @poly1 );
SELECT srcontainsl( @poly1 , @poly4 );
SELECT srcontainsl( @poly4 , @poly1 );
SELECT soverlaps( @poly1, @poly4 );
SELECT soverlaps( @poly4, @poly1 );
SELECT srcontainsl( @poly1 , @poly5 );
SELECT srcontainsl( @poly5 , @poly1 );
SELECT soverlaps( @poly1, @poly5 );
SELECT soverlaps( @poly5, @poly1 );

--  From testsuite/poly_test.sql

SET sphere_outmode="DEG";

SELECT sstr( spoly( '{(10d,0d),(10d,1d),(15d,0d)}' ) );

SELECT sstr( spoly( '{(359d,0d),(359d,1d),(4d,0d)}' ) );

SELECT sstr( spoly( '{(10d,0d),(10d,1d),(15d,0d)}' ) );

-- - functions

SELECT snpoints( spoly( '{(10d,0d),(10d,1d),(15d,0d)}') );

SELECT snpoints( spoly( '{(10d,0d),(10d,1d),(15d,0d),(5d,-5d)}') );

-- SELECT npoints( spoly( '{(0d,0d),(0d,90d),(15d,90d),(15d,0d)}');

SELECT sarea(spoly( '{(0d,0d),(0d,90d),(1,0d)}') );

SELECT sarea(spoly( '{(0d,0d),(0d,90d),(90d,0d)}'))/(4.0*pi());

-- - operations

-- -, osequal( perato )r

-- - should be true

SELECT sequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(1d,1d),(2d,1d),(1d,0d)}' ) );

SELECT sequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(2d,1d),(1d,1d),(1d,0d)}' ) );

SELECT sequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(1d,0d),(0d,0d),(0d,1d),(1d,1d)}' ) );

SELECT sequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(0d,0d),(1d,0d),(1d,1d),(0d,1d)}' ) );

-- - should be false

SELECT sequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(1d,1d),(3d,1d),(1d,0d)}' ) );

SELECT sequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(1d,0d),(0d,0d),(0d,1d),(2d,2d)}' ) );

-- -, snotequal( operato )r

-- - should be false

SELECT snotequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(1d,1d),(2d,1d),(1d,0d)}' ) );

SELECT snotequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(2d,1d),(1d,1d),(1d,0d)}' ) );

SELECT snotequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(1d,0d),(0d,0d),(0d,1d),(1d,1d)}' ) );

SELECT snotequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(0d,0d),(1d,0d),(1d,1d),(0d,1d)}' ) );

-- - should be true

SELECT snotequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(1d,1d),(3d,1d),(1d,0d)}' ) );

SELECT snotequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(1d,0d),(0d,0d),(0d,1d),(2d,2d)}' ) );

-- - sposrcontainsl( int, spol )y

-- - should be true
SELECT snotequal( spoly( '{(1d,0d),(1d,1d),(2d,1d)}' ), spoly( '{(1d,1d),(3d,1d),(1d,0d)}' ) );

SELECT snotequal( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(1d,0d),(0d,0d),(0d,1d),(2d,2d)}' ) );

-- - sposrcontainsl( int, spol )y

-- - should be true

SELECT srcontainsl( spoint( '(0.5d,0.5d)' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( spoint( '(0d,0.5d)' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( spoint( '(0d,0d)' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( spoint( '(0.5d,0.5d)' ), spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( spoint( '(0d,89.9d)' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( spoint( '(0d,90d)' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( spoint( '(0d,-89.9d)' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT srcontainsl( spoint( '(0d,-90d)' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

-- - should be false

SELECT srcontainsl( spoint( '(0.1d,0.5d)' ), spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( spoint( '(45d,-89d)' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT srcontainsl( spoint( '(0d,1d)' ), spoly( '{(0d,0d),(1d,1d),(1d,0d)}' ) );

-- - sposlcontainsr( ly, spoin )t

-- - should be true

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoint( '(0.5d,0.5d)' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoint( '(0d,0.5d)' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoint( '(0d,0d)' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ), spoint( '(0.5d,0.5d)' ) );

SELECT slcontainsr(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoint( '(0d,89.9d)' ) );

SELECT slcontainsr(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoint( '(0d,90d)' ) );

SELECT slcontainsr(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), spoint( '(0d,-89.9d)' ) );

SELECT slcontainsr(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), spoint( '(0d,-90d)' ) );

-- - should be false

SELECT slcontainsr(  spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ), spoint( '(0.1d,0.5d)' ) );

SELECT slcontainsr(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), spoint( '(45d,-89d)' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(1d,1d),(1d,0d)}' ), spoint( '(0d,1d)' ) );

-- - scisrcontainsl( rcle, spol )y

-- - should be true

SELECT srcontainsl( scircle( '<(0.5d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( scircle( '<(0d,89.9d),0.1d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( scircle( '<(0d,90d),0.1d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( scircle( '<(0d,-89.9d),0.1d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT srcontainsl( scircle( '<(0d,-90d),0.1d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

-- - should be false

SELECT srcontainsl( scircle( '<(0.1d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( scircle( '<(45d,-89d),0.1d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT srcontainsl( scircle( '<(0d,1d),0.1d>' ), spoly( '{(0d,0d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( scircle( '<(0d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( scircle( '<(0d,0d),0.1d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( scircle( '<(0.5d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ) );

-- - sposlcontainsr( ly, scircl )e

-- - should be true

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.5d,0.5d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,89.9d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,90d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(0d,-89.9d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(0d,-90d),0.1d>' ) );

--  should be false

SELECT slcontainsr(  spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.1d,0.5d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(45d,-89d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(1d,1d),(1d,0d)}' ), scircle( '<(0d,1d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0d,0.5d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0d,0d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.1d,0.5d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.1d,0.1d),0.1d>' ) );

SELECT slcontainsr(  spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.6d,0.5d),0.1d>' ) );

-- - sposrcontainsl( ly, scircl )e

-- - should be true

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0d,0d),2.0d>' ) );

SELECT srcontainsl( spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ), scircle( '<(0d,0d),1.0d>' ) );

SELECT srcontainsl( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,90d),1.0d>' ) );

SELECT srcontainsl( spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(180d,-90d),1.0d>' ) );

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ), scircle( '<(0d,0d),1.0d>' ) );

-- - should be false

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0d,0d),1.0d>' ) );

SELECT srcontainsl( spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ), scircle( '<(0d,0d),0.99d>' ) );

SELECT srcontainsl( spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ), scircle( '<(60d,0d),0.99d>' ) );

SELECT srcontainsl( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,88d)}' ), scircle( '<(0d,90d),1.0d>' ) );

SELECT srcontainsl( spoly( '{(0d,-87d),(90d,-87d),(180d,-87d),(270d,-87d)}' ), scircle( '<(180d,-90d),1.0d>' ) );

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(2d,0d)}' ), scircle( '<(0d,0d),1.0d>' ) );

-- - scislcontainsr( rcle, spol )y

-- - should be true

SELECT slcontainsr( scircle( '<(0d,0d),2.0d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT slcontainsr( scircle( '<(0d,0d),1.0d>' ), spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ) );

SELECT slcontainsr( scircle( '<(0d,90d),1.0d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT slcontainsr( scircle( '<(180d,-90d),1.0d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT slcontainsr( scircle( '<(0d,0d),1.0d>' ), spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ) );

-- - should be false

SELECT slcontainsr( scircle( '<(0d,0d),1.0d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT slcontainsr( scircle( '<(0d,0d),0.99d>' ), spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ) );

SELECT slcontainsr( scircle( '<(60d,0d),0.99d>' ), spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ) );

SELECT slcontainsr( scircle( '<(0d,90d),1.0d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,88d)}' ) );

SELECT slcontainsr( scircle( '<(180d,-90d),1.0d>' ), spoly( '{(0d,-87d),(90d,-87d),(180d,-87d),(270d,-87d)}' ) );

SELECT slcontainsr( scircle( '<(0d,0d),1.0d>' ), spoly( '{(0d,0d),(0d,1d),(2d,0d)}' ) );

-- - scisoverlaps( rcle, spol )y

-- - should be true

SELECT soverlaps(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.5d,0.5d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,89.9d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,90d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(0d,-89.9d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(0d,-90d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0d,0d),2.0d>' ) );

SELECT soverlaps(  spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ), scircle( '<(0d,0d),1.0d>' ) );

SELECT soverlaps(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,90d),1.0d>' ) );

SELECT soverlaps(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(180d,-90d),1.0d>' ) );

SELECT soverlaps(  spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ), scircle( '<(0d,0d),1.0d>' ) );

SELECT soverlaps(  spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), scircle( '<(0d,2d),1.0d>' ) );

SELECT soverlaps(  spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), scircle( '<(2d,0d),1.0d>' ) );

SELECT soverlaps(  spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.5d,0.5d),0.1d>' ) );

-- - should be false

SELECT soverlaps(  spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(1.5d,0.5d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), scircle( '<(0d,88.0d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ), scircle( '<(0.3d,0.5d),0.1d>' ) );

SELECT soverlaps(  spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ), scircle( '<(0d,-87d),0.1d>' ) );

-- - sposoverlaps( ly, scircl )e

-- - should be true

SELECT soverlaps( scircle( '<(0.5d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT soverlaps( scircle( '<(0d,89.9d),0.1d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( scircle( '<(0d,90d),0.1d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( scircle( '<(0d,-89.9d),0.1d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT soverlaps( scircle( '<(0d,-90d),0.1d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT soverlaps( scircle( '<(0d,0d),2.0d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT soverlaps( scircle( '<(0d,0d),1.0d>' ), spoly( '{(-1d,0d),(0d,1d),(1d,0d),(0d,-1d)}' ) );

SELECT soverlaps( scircle( '<(0d,90d),1.0d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( scircle( '<(180d,-90d),1.0d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

SELECT soverlaps( scircle( '<(0d,0d),1.0d>' ), spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ) );

SELECT soverlaps( scircle( '<(0d,2d),1.0d>' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT soverlaps( scircle( '<(2d,0d),1.0d>' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT soverlaps( scircle( '<(0.5d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ) );

-- - should be false

SELECT soverlaps( scircle( '<(1.5d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT soverlaps( scircle( '<(0d,88.0d),0.1d>' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( scircle( '<(0.3d,0.5d),0.1d>' ), spoly( '{(0d,0d),(0.5d,0.5d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT soverlaps( scircle( '<(0d,-87d),0.1d>' ), spoly( '{(0d,-89d),(90d,-89d),(180d,-89d),(270d,-89d)}' ) );

-- - sposrcontainsl( ly, spol )y

-- - should be true

SELECT srcontainsl( spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT srcontainsl( spoly( '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT srcontainsl( spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT srcontainsl( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( spoly( '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

-- - should be false

-- SELECsrcontainsl( T spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT srcontainsl( spoly( '{(45d,89.2d),(135d,89.2d),(225d,89.2d),(315d,89.2d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(0.5d,0.5d),(0.5d,1.5d),(1.5d,1.5d),(1.5d,0.5d)}' ) );

SELECT srcontainsl( spoly( '{(0d,88d),(90d,88d),(180d,88d),(270d,88d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT srcontainsl( spoly( '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

-- - sposlcontainsr( ly, spol )y

-- - should be true                                                                                       

SELECT slcontainsr( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ) );

SELECT slcontainsr( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}' ) );

SELECT slcontainsr( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ) );

SELECT slcontainsr( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ) );

SELECT slcontainsr( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ) );

SELECT slcontainsr( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT slcontainsr( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}' ) );

-- - should be false

SELECT slcontainsr( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(45d,89.2d),(135d,89.2d),(225d,89.2d),(315d,89.2d)}' ) );

SELECT slcontainsr( spoly( '{(0.5d,0.5d),(0.5d,1.5d),(1.5d,1.5d),(1.5d,0.5d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT slcontainsr( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,88d),(90d,88d),(180d,88d),(270d,88d)}' ) );

SELECT slcontainsr( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}' ) );

-- - sposoverlaps( ly, spol )y

-- - should be true                                                                                       

SELECT soverlaps( spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT soverlaps( spoly( '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT soverlaps( spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT soverlaps( spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ), spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ) );

SELECT soverlaps( spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ) );

SELECT soverlaps( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( spoly( '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(-0.5d,-0.5d),(-0.5d,0.5d),(0.5d,0.5d),(0.5d,-0.5d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(0d,0d),(0d,0.5d),(0.5d,0.5d),(0.5d,0d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ) );

SELECT soverlaps( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(0d,0d),(0d,1d),(1d,0d)}' ) );

SELECT soverlaps( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(45d,89.3d),(135d,89.3d),(225d,89.3d),(315d,89.3d)}' ) );

SELECT soverlaps( spoly( '{(45d,89.2d),(135d,89.2d),(225d,89.2d),(315d,89.2d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( spoly( '{(0d,0d),(0d,1d),(1d,1d),(1d,0d)}' ), spoly( '{(0.5d,0.5d),(0.5d,1.5d),(1.5d,1.5d),(1.5d,0.5d)}' ) );

SELECT soverlaps( spoly( '{(0d,88d),(90d,88d),(180d,88d),(270d,88d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

-- - should be false

SELECT soverlaps( spoly( '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}' ), spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ) );

SELECT soverlaps( spoly( '{(0d,89d),(90d,89d),(180d,89d),(270d,89d)}' ), spoly( '{(0d,-88d),(90d,-88d),(180d,-88d),(270d,-88d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(3d,-1d),(3d,1d),(5d,1d),(5d,-1d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(-1d,3d),(-1d,5d),(1d,5d),(1d,3d)}' ) );

SELECT soverlaps( spoly( '{(-1d,-1d),(-1d,1d),(1d,1d),(1d,-1d)}' ), spoly( '{(179d,-1d),(179d,1d),(181d,1d),(181d,-1d)}' ) );

--
-- ellipse and polygon
--
  
-- negators , commutator @,&&

SELECT srcontainsl( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )   ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' )   ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' )  ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ) ,  sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srnotcontainsl( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )  , sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srnotcontainsl( spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' )  , sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT srnotcontainsl( spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ) , sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT snotoverlaps( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) , sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT snotoverlaps( spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' ) , sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT snotoverlaps( spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ), sellipse( '<{10d,5d},(280d,-20d),90d>' ) );
SELECT slcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT slcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' ) );
SELECT slcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ) );
SELECT soverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ) );
SELECT slnotcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' ) , spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT slnotcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' ) , spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' ) );
SELECT slnotcontainsr( sellipse( '<{10d,5d},(280d,-20d),90d>' ) , spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ) );
SELECT snotoverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' ) , spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT snotoverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' ) , spoly( '{(280d, -9d),(280d,-12d),(279d, -8d)}' ) );
SELECT snotoverlaps( sellipse( '<{10d,5d},(280d,-20d),90d>' ) , spoly( '{(280d,-11d),(280d,-12d),(279d, -12d)}' ) );

-- ellipse is point
SELECT srcontainsl( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )   ,  sellipse( '<{0d,0d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' )  ,  sellipse( '<{0d,0d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )  ,  sellipse( '<{0d,0d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' ) ,  sellipse( '<{0d,0d},(280d,-20d),90d>' ) );
SELECT srcontainsl( sellipse( '<{0d,0d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT srcontainsl( sellipse( '<{0d,0d},(280d,-20d),90d>' )  ,  spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' ) );
SELECT soverlaps( sellipse( '<{0d,0d},(280d,-20d),90d>' ) ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{0d,0d},(280d,-20d),90d>' ) ,  spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' ) );

-- ellipse is circle
SELECT srcontainsl( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )   ,  sellipse( '<{5d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spoly( '{(280d,-10d),(290d,-30d),(270d, -30d)}' )  ,  sellipse( '<{2d,2d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )  ,  sellipse( '<{5d,5d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' ) ,  sellipse( '<{5d,5d},(280d,-20d),90d>' ) );
SELECT srcontainsl( sellipse( '<{5d,5d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT srcontainsl( sellipse( '<{2d,2d},(280d,-20d),90d>' )  ,  spoly( '{(280d,-10d),(290d,-30d),(270d, -30d)}' ) );
SELECT soverlaps( sellipse( '<{5d,5d},(280d,-20d),90d>' ) ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{5d,5d},(280d,-20d),90d>' ) ,  spoly( '{(280d,-11d),(280d,-18d),(279d, -12d)}' ) );

-- ellipse is line
SELECT srcontainsl( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )   ,  sellipse( '<{5d,0d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spoly( '{(280d,-10d),(290d,-30d),(270d, -30d)}' )  ,  sellipse( '<{2d,0d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )  ,  sellipse( '<{5d,0d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' ) ,  sellipse( '<{5d,0d},(280d,-20d),90d>' ) );
SELECT srcontainsl( sellipse( '<{5d,0d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT srcontainsl( sellipse( '<{2d,0d},(280d,-20d),90d>' )  ,  spoly( '{(280d,-10d),(290d,-30d),(270d, -30d)}' ) );
SELECT soverlaps( sellipse( '<{5d,0d},(280d,-20d),90d>' ) ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{5d,0d},(280d,-20d),90d>' ) ,  spoly( '{(280d,-11d),(280d,-18d),(279d, -12d)}' ) );

-- ellipse is a real ellipse
SELECT srcontainsl( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )   ,  sellipse( '<{5d,2d},(280d,-20d),90d>' ) );
SELECT srcontainsl( spoly( '{(280d,-10d),(290d,-30d),(270d, -30d)}' )  ,  sellipse( '<{2d,1d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' )  ,  sellipse( '<{5d,2d},(280d,-20d),90d>' ) );
SELECT soverlaps( spoly( '{(280d,-11d),(280d,-20d),(279d, -12d)}' ) ,  sellipse( '<{5d,2d},(280d,-20d),90d>' ) );
SELECT srcontainsl( sellipse( '<{5d,2d},(280d,-20d),90d>' )  ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT srcontainsl( sellipse( '<{2d,1d},(280d,-20d),90d>' )  ,  spoly( '{(280d,-10d),(290d,-30d),(270d, -30d)}' ) );
SELECT soverlaps( sellipse( '<{5d,2d},(280d,-20d),90d>' ) ,  spoly( '{(280d, -9d),(280d, -8d),(279d, -8d)}' ) );
SELECT soverlaps( sellipse( '<{5d,2d},(280d,-20d),90d>' ) ,  spoly( '{(280d,-11d),(280d,-18d),(279d, -12d)}' ) );

-- create polygon as aggregate
SELECT sstr( spoly_aggr(data.p) ) FROM ( SELECT spoint( '(0,1)' ) as p UNION ALL SELECT spoint( '(1,1)' ) UNION ALL SELECT spoint( '(1,0)' ) ) AS data ;

-- test stored data
SELECT count(polygon) FROM spherepolygon WHERE sarea(polygon) BETWEEN 5.735555 AND 5.735556 ;

-- check to create this small polygon without errors
SELECT sarea( spoly( '{
  (3.09472232280407 , 1.47261266025223),
  (3.0947320190777 , 1.47261266025223),
  (3.0947320190777 , 1.47262235652586),
  (3.09472232280407 , 1.47262235652586) }' ) ) >= 0 ;

SELECT snpoints( spoly( '{
  (1.51214841579108 , -2.90888208684947e-05),
  (1.5121581120647 , -2.90888208684947e-05),
  (1.5121581120647 , -1.93925472462553e-05),
  (1.51214841579108 , -1.93925472462553e-05)
}' ) );

-- incorrect input ----- 

SELECT spoly( '{(10d,0d),(10d,1d)}' );

-- -  self-crossing input ----- 

SELECT spoly( '{(0d,0d),(10d,10d),(0d,10d),(10d,0d)}' );
