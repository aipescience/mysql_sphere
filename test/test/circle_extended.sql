-- indexed operations.....
USE spheretest;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,90d),1.0d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,90d),1.0d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,90d),1.1d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,90d),1.1d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,-90d),1.0d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,-90d),1.0d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,-90d),1.1d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,-90d),1.1d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,0d),2.1d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,0d),2.1d>'), spoint)=1;



select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,90d),1.0d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,90d),1.0d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,90d),1.0d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,90d),1.1d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,90d),1.1d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,90d),1.1d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,-90d),1.0d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,-90d),1.0d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,-90d),1.0d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,-90d),1.1d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,-90d),1.1d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,-90d),1.1d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,0d),2.1d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,0d),2.1d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,0d),2.1d>'), scircle)=1;



-- SET enable_indexscan=on;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,90d),1.0d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,90d),1.0d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,90d),1.1d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,90d),1.1d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,-90d),1.0d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,-90d),1.0d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,-90d),1.1d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,-90d),1.1d>'), spoint)=1;

select count(spoint) from spherepoint where srcontainsl(spoint, scircle('<(0d,0d),2.1d>'))=1;

select count(spoint) from spherepoint where slcontainsr(scircle('<(0d,0d),2.1d>'), spoint)=1;



select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,90d),1.0d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,90d),1.0d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,90d),1.0d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,90d),1.1d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,90d),1.1d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,90d),1.1d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,-90d),1.0d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,-90d),1.0d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,-90d),1.0d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,-90d),1.1d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,-90d),1.1d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,-90d),1.1d>'), scircle)=1;

select count(scircle) from spherecircle where soverlaps(scircle, scircle('<(0d,0d),2.1d>'))=1;

select count(scircle) from spherecircle where srcontainsl(scircle, scircle('<(0d,0d),2.1d>'))=1;

select count(scircle) from spherecircle where slcontainsr( scircle('<(0d,0d),2.1d>'), scircle)=1;


-- "Cross-correlation" about 30 seconds on PIII-750
select count(spherepoint.spoint) from spherepoint,spherecircle where srcontainsl(spherepoint.spoint, spherecircle.scircle)=1;
