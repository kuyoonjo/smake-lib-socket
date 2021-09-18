import * as index from '../src/index';

test('Should have socket available', () => {
  expect(index.socket).toBeTruthy();
});
