import os
import time

from flask import Flask, render_template
import serial

def create_app(test_config=None):

    def inf_seq():
        num = 0
        while True:
            if num >= 10:
                break
            yield str(num)
            time.sleep(1)
            num += 1

    # create and configure the app
    app = Flask(__name__, instance_relative_config=True)
    app.config.from_mapping(
        SECRET_KEY='dev',
        DATABASE=os.path.join(app.instance_path, 'flaskr.sqlite'),
    )

    if test_config is None:
        # load the instance config, if it exists, when not testing
        app.config.from_pyfile('config.py', silent=True)
    else:
        # load the test config if passed in
        app.config.from_mapping(test_config)

    # ensure the instance folder exists
    try:
        os.makedirs(app.instance_path)
    except OSError:
        pass

    @app.route("/")
    def index():
        return render_template('base.html')

    @app.route("/teststream", methods=["GET"])
    def testStream():
        def generate():
            for row in inf_seq():
                yield row
        return app.response_class(generate())

    '''
    arduino = serial.Serial(port='/dev/cu.usbserial-14440', baudrate=9600, timeout=0.5)

    @app.route("/arduino-data")
    def readFromArduino():
        def generate():
            while True:
                line = arduino.readline().strip()

                if line:
                    yield line
        return app.response_class(generate())
    
    @app.route("/stop-arduino")
    def stopArduino():
        arduino.write(bytes("x\n", 'utf-8'))
        return ""
    '''

    return app