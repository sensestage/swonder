

global rresox=200
global rresoy=200

function mat=calc_circ( center_x, center_y, resox, resoy )
	mab=ones( resox, resoy );
	for x=1:resox
		for y=1:resoy
			mab( x, y ) = sqrt( (x - center_x)^2 + (y - center_y)^2 );
		endfor
	endfor
	mat=mab;
endfunction

function bool = fexists( name )
	f = fopen( name, "rb" );
	if( f != -1 )
		fclose( f );
		bool = 1;
	else 
		bool=0;
	endif
endfunction

function mat = load_precalc_circle()
	if( fexists( "circle.bin" ) )
		x = load "circle.bin";
		mat = x.mat;
	else
		mat = calc_circ( 400,400,800,800 );
		save -binary "circle.bin" mat;
	endif
endfunction

function mat=get_circ( center_x, center_y )
	mab=ones( 200, 200 );
	for x=1:200
		for y=1:200
			mab( x, y ) = sqrt( (x - center_x)^2 + (y - center_y)^2 );
		endfor
	endfor
	mat=mab;
endfunction

function mat=get_precalc_circ( center_x, center_y, precalc_circ )
	mat = precalc_circ( 400-center_x:599-center_x, 400-center_y:599-center_y );
#	mat = precalc_circ( 401-center_x:600-center_x, 401-center_y:600-center_y );
#	mat = precalc_circ( 399-center_x:598-center_x, 399-center_y:598-center_y );
endfunction

function mat=get_wave( center_x, center_y, phi, omega, precalc )
	m = get_precalc_circ( center_x, center_y, precalc );
	mat = (0.1*m+1).^-1  .* sin( 2*pi*omega*m + phi*omega*2*pi );
endfunction

colormap( gray(256) );


#precalculated_circle = calc_circ( 200,200,400,400 );
precalculated_circle = load_precalc_circle();

pic = zeros( 200, 200 );
pic2 = zeros( 200, 200 );
spkmap = zeros( 200, 200 );


spk = zeros( 24*4,2 );
spkn = zeros( 24*4,2 );
weights = ones( 24*4 );

for i=1:24
	spk(i,1) = 30;
	spk(i,2) = 30 + 5 * i;
	spkn(i,1) = 1;
	spkn(i,2) = 0;
endfor
	
for i=1:24
	spk(i+24,1) = 30 + 5 * i;
	spk(i+24,2) = 30;
	spkn(i+24,1) = 0;
	spkn(i+24,2) = 1;
endfor
	
##weights( 1 ) = 0.75;
##weights( 25 ) = 0.75;

function res = theint( l, r )
	 res = ( log( abs( sqrt( l^2 + r^2 ) + l )) * r^2 + l * sqrt( l^2+r^2 ) ) / 2;
endfunction

omeg = 0.25;

for i=1:48
	rvec = spk(i,:) - [50, 10];
	wrongrlen = sqrt( sumsq( rvec ) )
	normproject = dot( rvec, spkn(i,:) )
	if( normproject < 0 )
		wrongrlen = - wrongrlen
	endif

	cosphi = normproject / wrongrlen;

	rr = dot(rvec, spkn(i,:));
	l = dot(rvec, rot90( spkn(i,:) ) );
	l0 = l - 2.5;
	l1 = l + 2.5;

	rlen = (theint( l1, rr ) - theint( l0,rr )) / 5
	if( normproject < 0 )
		rlen = - rlen
	endif
	#rlen = wrongrlen;
	#cosphi = dot( rvec, spkn(i,:) ) / rlen;

	pic = pic + weights(i) /rlen * cosphi * get_wave( spk(i,1), spk(i,2), rlen, omeg, precalculated_circle );
	#pic2 = pic2 + weights(i) /wrongrlen * cosphi * get_wave( spk(i,1), spk(i,2), wrongrlen, 0.1, precalculated_circle );
	spkmap( spk(i,1), spk(i,2) ) =1.0;
endfor


#pic2 = get_wave( 50, 10, 0, omeg, precalculated_circle );
#pic2 = pic-pic2;


pic = pic .- min(min(pic));
pic = pic ./ max(max(pic));


pic2 = pic2 .- min(min(pic2));
pic2 = pic2 ./ max(max(pic2));

imshow( pic2, pic, spkmap );

