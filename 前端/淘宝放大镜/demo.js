function Index(mul) {
    this.dom = {
        wrap: $('.wrapper'),
        con: $('.content'),
        imgCov: $('.imgCover'),
        moveBox: $('.moveView'),
        bigView: $('.bigView'),
        oUl: $('.bottomBox ul')
    }
    this.mul = mul;
    this.init();
}
Index.prototype.init = function () {
    this.bindEvent();
    this.createView();    
}
Index.prototype.createView = function () {
    this.moveBoxW = 500 / this.mul;
    var width = this.moveBoxW;
    this.dom.moveBox.css({
        'width': width + 'px',
        'height': width + 'px',
    })
}
Index.prototype.getSize = function (index) {
    var self = this;
    var oUl = self.dom.oUl,
        styleCss;
    var img = new Image(),
        src = oUl.find('img').eq(index).attr('src');
    img.src = src;
    oUl.find('li').removeClass('active').eq(index).addClass('active');
    if (img.width > img.height) {
        self.imgWidth = 500;
        self.imgHeight = img.height / (img.width / 500);
        styleCss = 'top:50%;margin-top:' + (-self.imgHeight / 2) + 'px';
    } else {
        self.imgHeight = 500;
        self.imgWidth = img.width / (img.height / 500);
        styleCss = 'left:50%;margin-left:' + (-self.imgWidth / 2) + 'px';
    }
    self.dom.imgCov.empty()
        .append('<img src ="' + src + '"width="' +
        self.imgWidth + '"height="' + self.imgHeight + '"style="' + styleCss + '"/>');
    self.dom.bigView.empty()
        .append('<img src ="' + src + '"width="' +
        self.imgWidth * self.mul + '"height="' + self.imgHeight * self.mul + '"/>');
}
Index.prototype.move = function (e) {
    var self = this;
    var w = self.moveBoxW,
        minXl = (500 - self.imgWidth) / 2,
        maxXr = 500 - minXl - w + 1,
        minYt = (500 - self.imgHeight) / 2,
        maxYb = 500 - minYt - w + 1,
        wrap = self.dom.wrap,
        X = (e.clientX - wrap.offset().left) - w / 2,
        Y = (e.clientY - wrap.offset().top + $(document).scrollTop()) - w / 2,
        endX = (X > minXl) ? (X < maxXr) ? X : maxXr : minXl,
        endY = (Y > minYt) ? (Y < maxYb) ? Y : maxYb : minYt;

        
    self.dom.moveBox.css({
        'left': endX,
        'top': endY,
        'display': 'block',
    })
    var posX = (endX - (500 - self.imgWidth) / 2) * self.mul,
        posY = (endY - (500 - self.imgHeight) / 2) * self.mul;
    self.dom.bigView.css({
        'display':'block'
    }).find('img').css({
        'margin-left': -posX,
        'margin-top':-posY
    });
}
Index.prototype.bindEvent = function(){
    var self = this;
    self.getSize(0);
    self.dom.oUl.find('li').on('click',function(){
        index = $(this).index();
        self.getSize(index)
    });
    self.dom.con.on('mousemove',function(e){
        self.move(e);
    }).on('mouseleave',function(){
        self.dom.bigView.hide();
        self.dom.moveBox.hide();
    })
}

new Index(4);